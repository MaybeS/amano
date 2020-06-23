// LPR-visualize.cpp : Defines the entry point for the application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "resource.h"
#include "framework.h"

#include <vector>
#include <iostream>
#include <fstream>

#include "acognitive_post_def.h"
#include "parking.h"
#include "LibDetection.h"
#include "LibWarping.h"
#include "LibTracking.h"

using namespace std;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//
//   FUNCTION: GUI utility
//
Detector* g_Detector = nullptr;
std::vector<Box>* results = nullptr;
std::vector<Box>* Yresults = nullptr;

cv::Size shape_raw{ 1536, 2048 };
cv::Size shape_window{ 1536, 1024 + 60 };
cv::Size shape_image{ 768,1024 };
cv::Size shape_ground{ 768, 1024 };

char szFileName[256] = { 0, };

Gdiplus::Bitmap* image_map = nullptr, * ground_map = nullptr;
cv::Mat* image = nullptr, * ground = nullptr;
cv::Mat* image_resized = nullptr, * ground_resized = nullptr;
cv::Mat* crop = nullptr;
cv::Mat image_show, image_temp;

Calibration	*calib = nullptr;
Warp		*warp = nullptr;

BOOL		parking_draw = FALSE;
BOOL		parking_check = FALSE;
RECT		parking_rc{ 0, 0, 0, 0 };

AP_HANDLE	g_ApHandle = nullptr;
char		g_ACognitiveUrl[256] = {0};
BOOL		g_UnderFlip = FALSE;
HWND		g_hWnd = nullptr;

Tracker*	tracker = nullptr;
BOOL		video_stop = TRUE;
cv::VideoCapture capture;

class Color {
public:
	enum class Value {
		empty, parking, parking_invisible, 
	} value;

	Color() = default;
	constexpr Color(Value value)
		: value(value) {}
	constexpr Color(Park::State state)
		: value(static_cast<Value>(static_cast<int>(state))) {}

	operator Value() const { return value; }
	explicit operator bool() = delete;

	cv::Scalar color() {
		switch (value) {
			case Value::empty:
				return cv::Scalar{0, 0, 255};
			case Value::parking:
				return cv::Scalar{0, 255, 0};
			case Value::parking_invisible:
				return cv::Scalar{0, 255, 255};
		}
	}

	static cv::Scalar map(int seed = 0) {
		size_t MAP_SIZE = 210;
		size_t MAP[] = {
			59,76,192,60,78,194,61,80,195,62,81,197,63,83,
			198,64,85,200,66,87,201,67,88,203,68,90,204,69,
			92,206,70,93,207,71,95,209,73,97,210,74,99,211,
			75,100,213,76,102,214,77,104,215,79,105,217,
			80,107,218,81,109,219,82,110,221,84,112,222,
			85,114,223,86,115,224,87,117,225,89,119,226,
			90,120,228,91,122,229,93,123,230,94,125,231,
			95,127,232,96,128,233,98,130,234,99,131,235,
			100,133,236,102,135,237,103,136,238,104,138,239,
			106,139,239,107,141,240,108,142,241,110,144,242,
			111,145,243,112,147,243,114,148,244,115,150,245,
			116,151,246,118,153,246,119,154,247,120,156,247,
			122,157,248,123,158,249,124,160,249,126,161,250,
			127,163,250,129,164,251,130,165,251,131,167,252,
			133,168,252,134,169,252,135,171,253,137,172,253,
			138,173,253,140,174,254,141,176,254,142,177,254,
			144,178,254,145,179,254,147,181,255,148,182,255,
		};

		return cv::Scalar(
			MAP[seed % MAP_SIZE],
			MAP[(seed+1) % MAP_SIZE],
			MAP[(seed+2) % MAP_SIZE]
		);
	}
};

void resizeImage(int width, int height) {
	shape_raw.width = width;
	shape_raw.height = height;
}

void resizeWindow(int width, int height) {
	float ratio = shape_raw.height / (float)shape_raw.width;
	
	height = (int)(width * ratio / 2.f);
	width = height / ratio * 2.f;

	int image_width = (int)(width / 2.f);
	if ((image_width * 3) % 4) {
		image_width += ((image_width * 3) % 4);
	}
	shape_image.width = image_width;
	shape_image.height = height;

	int ground_width = (int)(height * (1920.f / 1080.f));
	if ((ground_width * 3) % 4) {
		ground_width += ((ground_width * 3) % 4);
	}
	shape_ground.width = ground_width;
	shape_ground.height = height;

	shape_window.height = height + 60;
	shape_window.width = image_width + ground_width;

	if (tracker != nullptr) {
		TrackerSetParam(tracker, shape_image.width ,shape_image.height);
	}

//	calib = new Calibration(shape_raw);
//	warp = new Warp(*calib, shape_window);
}

BOOL openFile(function<void(const OPENFILENAMEA&)> callback, const char* filter = nullptr) {
	OPENFILENAMEA openFileDialog;

	strcpy(szFileName, "");

	if (filter == nullptr) {
		filter = "Image (*.jpg)|*.jpg|*.png";
	}

	ZeroMemory(&openFileDialog, sizeof(openFileDialog));
	openFileDialog.lStructSize = sizeof(openFileDialog);
	openFileDialog.hwndOwner = nullptr;
	openFileDialog.lpstrFilter = filter;
	openFileDialog.lpstrFile = szFileName;
	openFileDialog.nMaxFile = MAX_PATH;
	openFileDialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileNameA(&openFileDialog)) {
		struct stat buffer;
		if (stat(openFileDialog.lpstrFile, &buffer) == 0) {
			callback(openFileDialog);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

void saveFile(function<void(const OPENFILENAMEA&)> callback, const char* filter = nullptr) {
	OPENFILENAMEA openFileDialog;

	// DEBUG
	// strcpy(szFileName, "calib.txt");
	int len = strlen(szFileName);
	szFileName[len - 3] = 't';
	szFileName[len - 2] = 'x';
	szFileName[len - 1] = 't';

	if (filter == nullptr) {
		filter = "Calibration Setting (*.txt)|*.txt";
	}

	ZeroMemory(&openFileDialog, sizeof(openFileDialog));
	openFileDialog.lStructSize = sizeof(openFileDialog);
	openFileDialog.hwndOwner = nullptr;
	openFileDialog.lpstrFilter = filter;
	openFileDialog.lpstrFile = szFileName;
	openFileDialog.nMaxFile = MAX_PATH;
	openFileDialog.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetSaveFileNameA(&openFileDialog)) {
		callback(openFileDialog);
	}
}

void indicator(HWND hWnd) {
	wstring buffer;
	wstringstream wss;
	HMENU menu = GetMenu(hWnd);

	wss.str(wstring());
	wss << "Center: (" << calib->cx << ", " << calib->cy << ")";
	ModifyMenu(menu, ID_CENTER, MF_BYCOMMAND | MF_STRING, ID_CENTER, wss.str().c_str());

	wss.str(wstring());
	wss << "F: (" << calib->fx << ", " << calib->fy << ")";
	ModifyMenu(menu, ID_F, MF_BYCOMMAND | MF_STRING, ID_F, wss.str().c_str());

	wss.str(wstring());
	wss << "CV: (" << setprecision(2) << warp->center.val[0] << ", " << warp->center.val[1] << ", " << warp->center.val[2] << ")";
	ModifyMenu(menu, ID_CV, MF_BYCOMMAND | MF_STRING, ID_CV, wss.str().c_str());

	wss.str(wstring());
	wss << "UV: (" << setprecision(2) << warp->up.val[0] << ", " << warp->up.val[1] << ", " << warp->up.val[2] << ")";
	ModifyMenu(menu, ID_UV, MF_BYCOMMAND | MF_STRING, ID_UV, wss.str().c_str());

	wss.str(wstring());
	wss << "RV: (" << setprecision(2) << warp->right.val[0] << ", " << warp->right.val[1] << ", " << warp->right.val[2] << ")";
	ModifyMenu(menu, ID_RV, MF_BYCOMMAND | MF_STRING, ID_RV, wss.str().c_str());

	DrawMenuBar(hWnd);
}

void redraw(HWND hWnd, bool inference = false, bool aspect = false) {
	warp->Map(*image, ground);

	image->copyTo(*image_resized);
	warp->DrawBoundingBox(image_resized);

	cv::resize(*image_resized, *image_resized, shape_image);
	cv::resize(*ground, *ground_resized, shape_ground);

	if (g_UnderFlip) {
		cv::imwrite("pimg_0.jpg", *ground_resized);
		cv::Mat tUnder = (*ground_resized)(cv::Rect(0, ground_resized->rows / 2, ground_resized->cols, ground_resized->rows / 2));
		cv::rotate(tUnder, tUnder, ROTATE_180);
		cv::imwrite("pimg_1.jpg", *ground_resized);
	}

	if (inference && g_Detector) {
		if (aspect) {
			DetectorSetParam(g_Detector, 1600, 300);
			int width = shape_ground.width;
			int height = shape_ground.height;

			cv::Mat up = (*ground_resized)(cv::Rect{ 0, 0, width, (int)(height / 3)});
			cv::Mat down, down_ = (*ground_resized)(cv::Rect{ 0, (int)(height * 2 / 3), width, (int)(height / 3)});
			cv::flip(down_, down, 0);

			if (results != nullptr) {
				delete results;
				results = nullptr;
			}

			results = new vector<Box>();
			for (const auto& element : *DetectorDetect(g_Detector, up)) {
				results->emplace_back(element);
			}
			for (auto& element : *DetectorDetect(g_Detector, down)) {
				element.y = height - element.y2;
				results->emplace_back(element);
			}

		} else {
			DetectorSetParam(g_Detector, 300, 300);
			results = DetectorDetect(g_Detector, *ground_resized);
		}
	}

	image_map = new Gdiplus::Bitmap(
		shape_image.width, shape_image.height, image_resized->step1(),
		PixelFormat24bppRGB, image_resized->data);

	ground_resized->copyTo(image_show);
	if (results != nullptr) {
		for (auto box : *results) {
			if (box.prob > .3f) {
				cv::rectangle(image_show, {
					static_cast<int>(box.x), static_cast<int>(box.y),
					static_cast<int>(box.x2 - box.x), static_cast<int>(box.y2 - box.y),
				}, cv::Scalar(255, 0, 0), 2);
			}
		}
	}

	if (Yresults) {
		for (auto box : *Yresults) {
			cv::rectangle(image_show, {
				static_cast<int>(box.x), static_cast<int>(box.y),
				static_cast<int>(box.x2 - box.x), static_cast<int>(box.y2 - box.y),
			}, cv::Scalar(0, 0, 255), 2);
		}

		cv::imwrite("pimg_2.jpg", image_show);
	}

	TrackerViewParks(tracker, [](const Park& park) {
		cv::rectangle(image_show, {
			static_cast<int>(park.x * shape_image.width),
			static_cast<int>(park.y * shape_image.width),
			static_cast<int>((park.x2 - park.x) * shape_image.width),
			static_cast<int>((park.y2 - park.y) * shape_image.width),
		}, Color(park.state).color(), 2);
	});

	ground_map = new Gdiplus::Bitmap(
		shape_ground.width, shape_ground.height, image_show.step1(),
		PixelFormat24bppRGB, image_show.data);

	InvalidateRect(hWnd, NULL, NULL);
}
//
//   FUNCTION: GUI utility
//


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	
	// Initialize gdiplus
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DETECTOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
	
	// Initialize calibration parameter
	image_resized = new cv::Mat();
	ground_resized = new cv::Mat();

	if (!openFile([&](const OPENFILENAMEA& openFileDialog) {
		image = new cv::Mat(cv::imread(string(openFileDialog.lpstrFile), cv::IMREAD_COLOR));
		ground = new cv::Mat();

		resizeImage(image->cols, image->rows);
		resizeWindow(image->cols, image->rows);

		calib = CalibrationInit(shape_raw);
		warp = WarpInit(*calib, shape_window);

		// Init tracker
		tracker = TrackerInit(shape_image.width, shape_image.height);

		// Load calib config sequentialy
		int len = strlen(openFileDialog.lpstrFile);
		openFileDialog.lpstrFile[len - 3] = 't';
		openFileDialog.lpstrFile[len - 2] = 'x';
		openFileDialog.lpstrFile[len - 1] = 't';
		std::ifstream in(openFileDialog.lpstrFile);

		if (in.is_open()) {
			in >> calib->cx >> calib->cy >> calib->fx >> calib->fy;

			in >> warp->center[0] >> warp->center[1] >> warp->center[2];
			in >> warp->up[0] >> warp->up[1] >> warp->up[2];
			in >> warp->right[0] >> warp->right[1] >> warp->right[2];
			in.close();

			warp->Update();
		}
	})) {
		return FALSE;;
	}

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DETECTOR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	
	// Destroy gdiplus
	Gdiplus::GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DETECTOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DETECTOR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, (int)shape_window.width, (int)shape_window.height,
	   nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

bool adjust_flag = false;

long _stdcall RequestImageCallBack(TRequestImage* AImageResult, WPARAM wParam)
{
	int tSize = AImageResult->m_ResultCount;

	if (tSize > 0 && !Yresults)
		Yresults = new std::vector<Box>;
	if (Yresults) Yresults->clear();

	for (int i = 0; i < tSize; i++)
	{
		TBbox &tBbox = AImageResult->m_Result[i];

		if (Yresults) {
			Box tPushBox;
			tPushBox.x = tBbox.x;
			tPushBox.y = tBbox.y;
			tPushBox.x2 = tBbox.x + tBbox.w;
			tPushBox.y2 = tBbox.y + tBbox.h;
			Yresults->push_back(tPushBox);
		}

	}

	return 0;
}

// 0: 모바일넷만
// 1: Y검지만
// 2: 둘다
void CarDetect(int AType)
{
	// acogurl.txt
	FILE* tf = fopen("acogurl.txt", "rt");
	if (tf) {
		fread(g_ACognitiveUrl, 1, sizeof(g_ACognitiveUrl), tf);
		fclose(tf);
		tf = nullptr;
	}

	

	if (AType == 1 || AType == 2)
	{
		if (strlen(g_ACognitiveUrl) <= 0)
		{
			::MessageBoxA(g_hWnd, "acogurl.txt에 Y검지URL이 없습니다. 확인부탁드립니다.", "확인", MB_OK);
			return;
		}

		if (ground_resized)
		{
			/////////////////////////////////////////////
			std::vector<int> compression_params;
			int tJpgCompression = 90;
			// For JPEG, it can be a quality ( CV_IMWRITE_JPEG_QUALITY ) from 0 to 100 (the higher is the better). Default value is 95.
			compression_params.push_back(1);// CV_IMWRITE_JPEG_QUALITY);
			compression_params.push_back(tJpgCompression);
			// 새로 이미지처리 Acognitive 요청.
			vector<uchar> buf;
			cv::imencode(".jpg", *ground_resized, buf, compression_params);

			ap_Request_ImageBuf(g_ApHandle, g_ACognitiveUrl, buf.data(), buf.size(), "detector_test", FALSE);
		}
	}

	if (AType == 0 || AType == 2)
	{
		redraw(g_hWnd, true);
	}	
}

void video_sequence(HWND hWnd) {
	cv::Mat frame;

	while (!video_stop) {
		if (!capture.isOpened()) {
			break;
		}
		capture >> frame;

		if (frame.empty()) {
			capture.release();
			break;
		}

		frame.copyTo(*image);
		redraw(hWnd);
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_hWnd = hWnd;

    switch (message)
    {
	case WM_CREATE:
		{
			g_ApHandle = ap_Init(RequestImageCallBack, (WPARAM)0);

			if (g_Detector != nullptr) {
				DetectorRelease(g_Detector);
			}

			g_Detector = DetectorInit("./bin/amano-script.pt");

			// acogurl.txt
			FILE* tf = fopen("acogurl.txt", "rt");
			if (tf) {
				fread(g_ACognitiveUrl, 1, sizeof(g_ACognitiveUrl), tf);
				fclose(tf);
				tf = nullptr;
			}

		//	redraw(hWnd);
		}	
		break;

	case WM_SIZE:
		{
			int nWidth = LOWORD(lParam);
			int nHeight = HIWORD(lParam);

			if (!adjust_flag) {
				adjust_flag = true;
				resizeWindow(nWidth, nHeight);
				SetWindowPos(hWnd, NULL, 0, 0, shape_window.width, shape_window.height,
					SWP_NOSENDCHANGING | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
				redraw(hWnd);
			} else {
				adjust_flag = false;
			}
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_DETECT_LOAD:
				openFile([&](const OPENFILENAMEA& openFileDialog) {
					if (g_Detector != nullptr) {
						DetectorRelease(g_Detector);
					}

					g_Detector = DetectorInit(openFileDialog.lpstrFile);
				});
				break;
			case ID_DETECT_INFERENCE:
			case ID_DETECT_INFERENCE_ASPECT:
				{
					if (g_Detector == nullptr)
						break;

					if (results != nullptr) {
						delete results;
						results = nullptr;
					}

					redraw(hWnd, true, wmId == ID_DETECT_INFERENCE_ASPECT);
				}
				break;
			case ID_DETECT_SAVE:
				saveFile([&](const OPENFILENAMEA& saveFileDialog) {
					std::ofstream out(saveFileDialog.lpstrFile);
					if (out.is_open() && results != nullptr) {
						for (auto box : *results) {
							out << std::fixed << box.prob
								<< box.x << " " << box.y << " "
								<< box.x2 << " " << box.y2 << std::endl;
						}
					}
					out.close();
				}, "Bounding boxes (.txt)|*.txt");
				break;

			case ID_DETECT_CLEAR:
				if (results != nullptr) {
					delete results;
					results = nullptr;
				}
				redraw(hWnd);
				break;

			case ID_FILE_OPEN:
				openFile([&](const OPENFILENAMEA& openFileDialog) {
					if (image != nullptr) {
						delete image;
						image = nullptr;
					}
					if (ground != nullptr) {
						delete ground;
						ground = nullptr;
					}
					image = new cv::Mat(cv::imread(string(openFileDialog.lpstrFile), cv::IMREAD_COLOR));
					ground = new cv::Mat();

					//resizeImage(image->cols, image->rows);
					//resizeWindow(image->cols, image->rows);

					SetWindowPos(hWnd, NULL, 0, 0, shape_window.width, shape_window.height, SWP_NOMOVE | SWP_NOREPOSITION);
					UpdateWindow(hWnd);
					// Load calib config sequentialy
					int len = strlen(openFileDialog.lpstrFile);
					openFileDialog.lpstrFile[len - 3] = 't';
					openFileDialog.lpstrFile[len - 2] = 'x';
					openFileDialog.lpstrFile[len - 1] = 't';
					std::ifstream in(openFileDialog.lpstrFile);

					if (in.is_open()) {
						in >> calib->cx >> calib->cy >> calib->fx >> calib->fy;

						in >> warp->center[0] >> warp->center[1] >> warp->center[2];
						in >> warp->up[0] >> warp->up[1] >> warp->up[2];
						in >> warp->right[0] >> warp->right[1] >> warp->right[2];
						in.close();

						warp->Update();
						indicator(hWnd);
					}
					redraw(hWnd);
				});
				break;
			case ID_FILE_LOAD:
				openFile([&](const OPENFILENAMEA& openFileDialog) {
					std::ifstream in(openFileDialog.lpstrFile);

					if (in.is_open()) {
						in >> calib->cx >> calib->cy >> calib->fx >> calib->fy;

						in >> warp->center[0] >> warp->center[1] >> warp->center[2];
						in >> warp->up[0] >> warp->up[1] >> warp->up[2];
						in >> warp->right[0] >> warp->right[1] >> warp->right[2];
					}

					in.close();

					warp->Update();
					indicator(hWnd);
					redraw(hWnd);
				}, "Calibration Setting (*.txt)|*.txt");
				break;
			case ID_FILE_SAVE:
				saveFile([&](const OPENFILENAMEA& saveFileDialog) {
					std::ofstream out(saveFileDialog.lpstrFile);

					if (out.is_open()) {
						out << std::fixed
							<< calib->cx << " " << calib->cy << " "
							<< calib->fx << " " << calib->fy << std::endl;
						out << std::fixed
							<< warp->center[0] << " " << warp->center[1] << " " << warp->center[2] << " " << std::endl;
						out << std::fixed
							<< warp->up[0] << " " << warp->up[1] << " " << warp->up[2] << " " << std::endl;
						out << std::fixed
							<< warp->right[0] << " " << warp->right[1] << " " << warp->right[2] << " " << std::endl;
					}

					out.close();
				}, "Calibration Setting (*.txt)|*.txt");
				break;

			case ID_PARKING_DRAW:
				{
					wstring buffer;
					wstringstream wss;

					HMENU menu = GetMenu(hWnd);

					wss.str(wstring());

					if (parking_draw) {
						wss << "Draw";
					} else {
						wss << "Drawinig";
					}
					parking_draw = !parking_draw;

					ModifyMenu(menu, ID_PARKING_DRAW, MF_BYCOMMAND | MF_STRING,
						ID_PARKING_DRAW, wss.str().c_str());
					DrawMenuBar(hWnd);
				}
				break;

			case ID_PARKING_CHECK:
				{
					wstring buffer;
					wstringstream wss;

					HMENU menu = GetMenu(hWnd);

					wss.str(wstring());

					if (parking_check) {
						wss << "Check";
					}
					else {
						wss << "Checking";
					}
					parking_check = !parking_check;

					ModifyMenu(menu, ID_PARKING_CHECK, MF_BYCOMMAND | MF_STRING,
						ID_PARKING_CHECK, wss.str().c_str());
					DrawMenuBar(hWnd);
				}
				break;

			case ID_PARKING_LOAD:
				openFile([&](const OPENFILENAMEA& openFileDialog) {
					std::ifstream in(openFileDialog.lpstrFile);

					TrackerClearParking(tracker);
					if (in.is_open()) {
						float x, y, x2, y2;
						while (in >> x >> y >> x2 >> y2) {
							TrackerAddParking(tracker, x, y, x2, y2);
						}
					}

					in.close();
					redraw(hWnd);
					}, "Parking boxes (.txt)|*.txt");
				break;

			case ID_PARKING_SAVE:
				saveFile([&](const OPENFILENAMEA& saveFileDialog) {
					std::ofstream out(saveFileDialog.lpstrFile);
					if (out.is_open()) {
						TrackerViewParks(tracker, [&out](const Park& park) {
							out << std::fixed
								<< park.x << " " << park.y << " "
								<< park.x2 << " " << park.y2 << std::endl;
						});
					}
					out.close();
				}, "Parking boxes (.txt)|*.txt");
				break;

			case ID_PARKING_CLEAR:
				TrackerClearParking(tracker);
				redraw(hWnd);
				break;

			case ID_TRACKING_LOADVIDEO:
				openFile([&](const OPENFILENAMEA& openFileDialog) {
					if (!capture.isOpened()) {
						capture = cv::VideoCapture{ openFileDialog.lpstrFile };

						if (capture.isOpened()) {
							cv::Mat frame;
							capture >> frame;

							if (frame.empty()) {
								return;
							}

							if (image != nullptr) {
								delete image;
								image = nullptr;
							}
							if (ground != nullptr) {
								delete ground;
								ground = nullptr;
							}

							image = new cv::Mat();
							ground = new cv::Mat();
							frame.copyTo(*image);

							resizeImage(frame.cols, frame.rows);
							resizeWindow(frame.cols, frame.rows);
							SetWindowPos(hWnd, NULL, 0, 0, shape_window.width, shape_window.height, SWP_NOMOVE | SWP_NOREPOSITION);
							UpdateWindow(hWnd);

							{ // if parking info exists
								int len = strlen(openFileDialog.lpstrFile);
								openFileDialog.lpstrFile[len - 3] = 't';
								openFileDialog.lpstrFile[len - 2] = 'x';
								openFileDialog.lpstrFile[len - 1] = 't';
								std::ifstream in(openFileDialog.lpstrFile);

								TrackerClearParking(tracker);
								if (in.is_open()) {
									float x, y, x2, y2;
									while (in >> x >> y >> x2 >> y2) {
										TrackerAddParking(tracker, x, y, x2, y2);
									}
								}

								in.close();
							}

							TrackerClearTracking(tracker);
							video_stop = TRUE;

							redraw(hWnd);
						}
					}
				}, "Video (.mp4)|*.mp4");
				break;

			case ID_TRACKING_LOADTRACKINGSTATE:
				openFile([&](const OPENFILENAMEA& openFileDialog) {
					std::ifstream in(openFileDialog.lpstrFile);

					TrackerClearParking(tracker);
					if (in.is_open()) {
						int state;
						float x, y, x2, y2;
						while (in >> x >> y >> x2 >> y2 >> state) {
							TrackerAddParking(tracker, x, y, x2, y2,
											  static_cast<Park::State>(state));
						}
					}

					in.close();
					redraw(hWnd);
					}, "Tracking States(.txt)|*.txt");
				break;

			case ID_TRACKING_SAVETRACKINGSTATE:
				saveFile([&](const OPENFILENAMEA& saveFileDialog) {
					std::ofstream out(saveFileDialog.lpstrFile);
					if (out.is_open()) {
						TrackerViewParks(tracker, [&out](const Park& park) {
							out << std::fixed
								<< park.x << " " << park.y << " "
								<< park.x2 << " " << park.y2 << " "
								<< static_cast<int>(park.state) << std::endl;
							});
					}
					out.close();
					}, "Tracking States (.txt)|*.txt");
				break;

			case ID_TRACKING_START:
			{
				wstring buffer;
				wstringstream wss;

				HMENU menu = GetMenu(hWnd);

				wss.str(wstring());

				if (parking_check) {
					wss << "Start";
				}
				else {
					wss << "Stop";
				}

				ModifyMenu(menu, ID_TRACKING_START, MF_BYCOMMAND | MF_STRING,
					ID_TRACKING_START, wss.str().c_str());
				DrawMenuBar(hWnd);

				video_stop = !video_stop;

				if (results != nullptr) {
					delete results;
					results = nullptr;
				}

				SetTimer(hWnd, 0, 5, NULL);
				SetTimer(hWnd, 1, 100, NULL);
				break;
			}

			case ID_ANGLE_3:
			case ID_ANGLE_2_8:
			case ID_ANGLE_2_6:
			case ID_ANGLE_2_4:
			case ID_ANGLE_2_2:
			{
				HMENU menu = GetMenu(hWnd);
				CheckMenuItem(menu, ID_ANGLE_3, MF_UNCHECKED);
				CheckMenuItem(menu, ID_ANGLE_2_8, MF_UNCHECKED);
				CheckMenuItem(menu, ID_ANGLE_2_6, MF_UNCHECKED);
				CheckMenuItem(menu, ID_ANGLE_2_4, MF_UNCHECKED);
				CheckMenuItem(menu, ID_ANGLE_2_2, MF_UNCHECKED);

				CheckMenuItem(menu, wmId, MF_CHECKED);

				warp->angle = static_cast<float>(((ID_ANGLE_2_2 - wmId + 1) * .2f) + 2);
				warp->Update();
				redraw(hWnd);
				break;
			}
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_ERASEBKGND:
		return TRUE;

    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);

			if (image_map != NULL && ground_map != NULL) {
				RECT rect;
				Gdiplus::Bitmap* render;

				GetClientRect(hWnd, &rect);

				Gdiplus::Bitmap buffer(rect.right, rect.bottom);
				Gdiplus::Graphics graphics_buffer(&buffer), graphics(hdc);

				// graphics.Clear(Gdiplus::Color::Black);
				render = image_map->Clone(
					Gdiplus::Rect(0, 0, shape_image.width, shape_image.height), PixelFormat24bppRGB);
				graphics_buffer.DrawImage(render, 0, 0);

				render = ground_map->Clone(
					Gdiplus::Rect(0, 0, shape_ground.width, shape_ground.height), PixelFormat24bppRGB);
				graphics_buffer.DrawImage(render, shape_image.width, 0);

				graphics.DrawImage(&buffer, 0, 0);
			}
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_KEYDOWN:
		{
			if (results) {
				delete results; 
				results = nullptr;
			}
			if (Yresults) {
				delete Yresults;
				Yresults = nullptr;
			}

			int ctrl_flag = GetAsyncKeyState(VK_CONTROL);
			int shift_flag = GetAsyncKeyState(VK_SHIFT);
			float flag = shift_flag ? .1f : 1.f;

			if (ctrl_flag) {
				switch (wParam) {
					case 'O':
						openFile([&](const OPENFILENAMEA& openFileDialog) {
							image = new cv::Mat(cv::imread(string(openFileDialog.lpstrFile), cv::IMREAD_COLOR));
							ground = new cv::Mat();

							resizeImage(image->cols, image->rows);
							resizeWindow(image->cols, image->rows);
							SetWindowPos(hWnd, NULL, 0, 0, shape_window.width, shape_window.height, SWP_NOMOVE | SWP_NOREPOSITION);
							UpdateWindow(hWnd);
							redraw(hWnd);
						});
						break;
					case 'S': 
						saveFile([&](const OPENFILENAMEA& saveFileDialog) {
							{
								std::ofstream out(saveFileDialog.lpstrFile);

								if (out.is_open()) {
									out << std::fixed
										<< calib->cx << " " << calib->cy << " "
										<< calib->fx << " " << calib->fy << std::endl;
									out << std::fixed
										<< warp->center[0] << " " << warp->center[1] << " " << warp->center[2] << " " << std::endl;
									out << std::fixed
										<< warp->up[0] << " " << warp->up[1] << " " << warp->up[2] << " " << std::endl;
									out << std::fixed
										<< warp->right[0] << " " << warp->right[1] << " " << warp->right[2] << " " << std::endl;
								}

								out.close();
							}
						}, "Calibration Setting (*.txt)|*.txt");
						break;
				}
			} else {
				switch (wParam) {
					// Mode (m)
					case 'M':	warp->plane_mode = (warp->plane_mode + 1) % 3; break;

						// Reset (r)
					case 'R':
						warp->Reset();
						calib->Reset();
						break;

						// Zoom (x,z)
					case 'X':	warp->Zoom(1.f + (.1f * flag)); break;
					case 'Z':	warp->Zoom(1.f - (.1f * flag)); break;

						// Rotate (q,e)
					case 'Q':	warp->Rotate(.1f * flag); break;
					case 'E':	warp->Rotate(-.1f * flag); break;

						// Translate cx, cy (w,a,s,d)
					case 'W':	calib->cy -= 5 * flag; break;
					case 'A':	calib->cx -= 5 * flag; break;
					case 'S':	calib->cy += 5 * flag; break;
					case 'D':	calib->cx += 5 * flag; break;

						// Aspect ratio fy (1,2=f, 3,4=fy)
					case '1':	calib->fx *= (1 + (.05f * flag)); break;
					case '3':	calib->fy *= (1 - (.05f * flag)); break;
					case '2':	calib->fx /= (1 + (.05f * flag)); break;
					case '4':	calib->fy /= (1 - (.05f * flag)); break;

					case '5':
					{
						// 밑의 절반 뒤집기.
						g_UnderFlip = !g_UnderFlip;

						break;
					}
					// 모바일넷만
					case '6':
						CarDetect(0);
						break;
						// Y검지만
					case '7':
						CarDetect(1);
						break;
						// 둘다
					case '8':
						CarDetect(2);
						break;
					default:
						break;
				}
			}

		}
		break;

	case WM_TIMER:
		switch (wParam) {
			case 0: // render video sequence
			{
				if (video_stop) {
					KillTimer(hWnd, wParam);
					break;
				}

				if (!capture.isOpened()) {
					capture.release();
					KillTimer(hWnd, wParam);
					break;
				}

				capture >> image_temp;

				if (image_temp.empty()) {
					capture.release();
					KillTimer(hWnd, wParam);
					break;
				}

				image_temp.copyTo(*image);
				redraw(hWnd);

				if (tracker != nullptr) {
					TrackerViewTracks(tracker, [](const Track& track) {
						cv::rectangle(image_show, {
							static_cast<int>(track.boxes.back().x),
							static_cast<int>(track.boxes.back().y),
							static_cast<int>(track.boxes.back().x2 - track.boxes.back().x),
							static_cast<int>(track.boxes.back().y2 - track.boxes.back().y),
						}, Color::map(track.id), 1);
					});
				}
				break;
			}
			case 1: // prediction if model loaded
			{
				if (video_stop) {
					KillTimer(hWnd, wParam);
					break;
				}

				if (g_Detector != nullptr && tracker != nullptr) {
					TrackerUpdate(tracker, *DetectorDetect(g_Detector, *ground_resized));
				}
			}
		}
		break;

	case WM_KEYUP:
		warp->Update();
		indicator(hWnd);
		redraw(hWnd);
		break;

	case WM_LBUTTONDOWN:
		{
			long X = LOWORD(lParam) - shape_image.width, Y = HIWORD(lParam);
			if (parking_draw) {
				parking_rc.left = X;
				parking_rc.top = Y;

				if (parking_rc.left < 0) {
					parking_rc.left = parking_rc.top = 0;
				}
			}

			if (parking_check) {
				float XX = X / static_cast <float>(shape_image.width);
				float YY = Y / static_cast <float>(shape_image.width);
				TrackerViewParks(tracker, [XX, YY](Park& park) {
					if (park.x <= XX && XX <= park.x2 && park.y <= YY && YY <= park.y2) {
						park.state = static_cast<Park::State>((static_cast<int>(park.state) + 1) % 3);
					}
				});
				redraw(hWnd);
			}
			break;
		}
	case WM_MOUSEMOVE:
		if (parking_draw) {
			if (parking_rc.left > 0 && parking_rc.top > 0) {
				parking_rc.right = LOWORD(lParam) - shape_image.width;
				parking_rc.bottom = HIWORD(lParam);
				cv::rectangle(image_show, {
					MIN(parking_rc.left, parking_rc.right),
					MIN(parking_rc.top, parking_rc.bottom),
				}, {
					MAX(parking_rc.left, parking_rc.right),
					MAX(parking_rc.top, parking_rc.bottom)
				}, Color(Park::State::empty).color(), 2);
				InvalidateRect(hWnd, NULL, NULL);
			}
			parking_rc.right = parking_rc.bottom = 0;
		}
		break;

	case WM_LBUTTONUP:
		if (parking_draw) {
			if (parking_rc.left > 0 && parking_rc.top > 0) {
				parking_rc.right = LOWORD(lParam) - shape_image.width;
				parking_rc.bottom = HIWORD(lParam);

				if (parking_rc.right >= 0) {
					TrackerAddParking(tracker,
						MIN(parking_rc.left, parking_rc.right) / static_cast<float>(shape_image.width),
						MIN(parking_rc.top, parking_rc.bottom) / static_cast<float>(shape_image.width),
						MAX(parking_rc.left, parking_rc.right) / static_cast<float>(shape_image.width),
						MAX(parking_rc.top, parking_rc.bottom) / static_cast<float>(shape_image.width)
					);
					redraw(hWnd);
				}
			}
			parking_rc.left = parking_rc.top = 0;
			parking_rc.right = parking_rc.bottom = 0;
		}
		break;

    case WM_DESTROY:
		DeleteObject(image_map);
		DeleteObject(ground_map);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);

    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
