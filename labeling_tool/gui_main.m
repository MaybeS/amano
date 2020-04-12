function varargout = gui_main(varargin)

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
    'gui_Singleton',  gui_Singleton, ...
    'gui_OpeningFcn', @gui_main_OpeningFcn, ...
    'gui_OutputFcn',  @gui_main_OutputFcn, ...
    'gui_LayoutFcn',  [] , ...
    'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before gui_main is made visible.
function gui_main_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to gui_main (see VARARGIN)

% Choose default command line output for gui_main
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

set(gcf,'units','normalized','outerposition',[1/4 1/4 3/4 3/4]);
set(handles.axes1,'visible','off');
xlabel(' ');
set(get(handles.axes1,'children'),'visible','off');
set(handles.axes3,'visible','off');
set(get(handles.axes3,'children'),'visible','off');
global d_img;
global u_img;
global panel_img_type;
global rays_set;

global patch_set_d;
global patch_set_u;

global isM;
global mIdx;

global objectClicked;
global find_mode;
global isAdd;

global file_name;
global file_idx;
global file_store;
file_store = '';
file_idx = -1;
file_name = '';

isAdd = false;
find_mode =false;
objectClicked = false;
isM = false;
mIdx =-1;

panel_img_type = ' ';
rays_set = {};
patch_set_d ={};
patch_set_u = {};



H =findall(gcf,'Tag','ID');
set(H,'String' , "");
H =findall(gcf,'Tag','Face');
set(H,'String' , "");
H =findall(gcf,'Tag','height');
set(H,'String' , "");
H =findall(gcf,'Tag','point1');
set(H,'String' , "");
H =findall(gcf,'Tag','point2');
set(H,'String' , "");
H =findall(gcf,'Tag','point3');
set(H,'String' , "");
set(handles.text2, 'String',"");
set(handles.text17, 'String' , "");
global fisheye_calib;


% UIWAIT makes gui_main wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = gui_main_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
global d_img;
global u_img;
global panel_img_type;
global rays_set;
global fisheye_calib;
global cp1;
global cp2;
global cp3;
delete(cp1);
delete(cp2);
delete(cp3);
r_sz = size(rays_set);

tmp_type = '';
switch panel_img_type
    %-- Frontal view
    case 'u';
        set(handles.axes1,'visible','off');
        set(get(handles.axes1,'children'),'visible','off');
        set(handles.axes3,'visible','on');
        set(get(handles.axes3,'children'),'visible','on');
        tmp_type = 'd';hold on;
        
        hold off;
        
    case 'd';
        set(handles.axes1,'visible','on');
        set(get(handles.axes1,'children'),'visible','on');
        set(handles.axes3,'visible','off');
        set(get(handles.axes3,'children'),'visible','off');
        tmp_type = 'u';hold on;
        hold off;
        
    otherwise ;  return;
end
panel_img_type = tmp_type;

%% --- Executes on button press in ADD_cube (3D Points)
function add3_Callback(hObject, eventdata, handles)
global panel_img_type;


global isAdd;
global rays_set;
global fisheye_calib;
global patch_set_u;
global patch_set_d;

isAdd = true;
K = [384 0 480 ; 0 384 240 ; 0 0 1]; %normal view intrinsics ..
w = 960; %normal view width
h = 480; %normal view heigt
th = 50*pi/180; %view angle
vh = 1.4;
% xlabel('바닥면 에 점 3개를 찍어주세요 (두 번째 점과 3번째 점이 이어지도록 찍어주세요)' , 'Color' ,'r');

[in_x , in_y] = ginput(3);
if panel_img_type =='d'
    axes(handles.axes3);
    rays = pix2ray(fisheye_calib, [in_x'; in_y']);
    view_idx = check_un_view(K , rays , w, h ,th);
    
    %make cube object
    tmp_obj = make_cube_object(K, rays, view_idx, vh , 'd' ,{});
    rays_set{end+1} = tmp_obj;
     axes(handles.axes3);
    hold on;
    draw_cube_fisheye(fisheye_calib, rays_set{end}.rays, @cube_ButtonDownFcn);
    xlabel(' ');
    hold off;
    
    axes(handles.axes1);
    hold on;
    %check again
    view_idx = check_un_view(K , rays_set{end}.rays(:,1:4), w, h ,th);
    pts_cube = get_cube_pts(rays_set{end}.calib,rays_set{end}.rays ,view_idx , w, h);
    draw_cube_un(pts_cube, @cube_ButtonDownFcn);
    hold off
    %set view
    set(handles.axes1,'visible','off');
    set(get(handles.axes1,'children'),'visible','off');

else
    axes(handles.axes1);
    
    %check where the points is annotated
    i_set = [inpolygon(in_x, in_y, [1 w w 1] , [1 1 h h]) ...
        inpolygon(in_x, in_y, [w 2*w w*2 w] , [1  1 h h]) ...
        inpolygon(in_x, in_y, [1 w w 1] , [h h 2*h 2*h]) ...
        inpolygon(in_x, in_y, [w 2*w 2*w w] , [h h 2*h 2*h])];
    
    [m, view_idx] = max(sum(i_set,1));
    
    if m == 0
%         xlabel('점을 잘못 치신것 같습니다 ...' );
        return;
    end
    tmp_obj = make_cube_object(K,[in_x'; in_y'], view_idx, vh , 'u',{});
    rays_set{end+1} = tmp_obj;
    axes(handles.axes1);
    hold on;
    pts_cube = get_cube_pts(rays_set{end}.calib,rays_set{end}.rays ,view_idx , w, h);
    draw_cube_un(pts_cube, @cube_ButtonDownFcn);
    xlabel(' ');
    hold off
    
    axes(handles.axes3);
    hold on;
    draw_cube_fisheye(fisheye_calib, rays_set{end}.rays , @cube_ButtonDownFcn);
    hold off;
    
    set(handles.axes3,'visible','off');
    set(get(handles.axes3,'children'),'visible','off');
end
isAdd = false;

%%
% --- Executes on button press in savebutton.
function savebutton_Callback(hObject, eventdata, handles)
save_csv(handles);
% --- If Enable == 'on', executes on mouse press in 5 pixel border.
% --- Otherwise, executes on mouse press in 5 pixel border or over savebutton.
function savebutton_ButtonDownFcn(hObject, eventdata, handles)

% --- Executes on button press in OpenBtn.
function OpenBtn_Callback(hObject, eventdata, handles)

global d_img;
global u_img;
global u_img_handle;
global d_img_handle;
global panel_img_type;
global rays_set;
global patch_set_u;
global patch_set_d;
global isClicked;
global isMove;
global mIdx;
global find_mode;
global isAdd;
global file_name;
global fisheye_calib ;
global file_store;
global file_idx;

% delete(u_img);
% delete(d_img);
fullpathname =" ";
[file_name pathname] = uigetfile({'*.jpg;*.tif;*.png;*.gif','All Image Files';...
    '*.*','All Files' },'File Selector');
fullpathname = strcat(pathname, file_name);
file_name = fullpathname;
if exist(fullpathname, 'file') ~= 2
    % File not exists.
    return;
end
isAdd = false;
find_mode =false;
mIdx = -1;
rays_set = {};
patch_set_u = {};
patch_set_d = {};
isClicked = false;
isMove =false;

file_store = imageDatastore(pathname);
file_idx = find(strcmp(file_store.Files,fullpathname));
direc_sz  = size(file_store.Files);
dstr =  file_idx+"/"+direc_sz;
set(handles.text17, 'String' , dstr);
H =findall(gcf,'Tag','ID');
set(H,'String' , "");
H =findall(gcf,'Tag','face');
set(H,'String' , "");
H =findall(gcf,'Tag','height');
set(H,'String' , "");
H =findall(gcf,'Tag','point1');
set(H,'String' , "");
H =findall(gcf,'Tag','point2');
set(H,'String' , "");
H =findall(gcf,'Tag','point3');
set(H,'String' , "");
set(handles.text2, 'String', fullpathname);
d_img = imread(fullpathname);

%calibration param initialize
I = mean(single(d_img), 3) / 255;
sz = fliplr(size(I));
f = 0.26 * sz(1);  % Fotec lens
c = sz / 2 + [77, 33];
k = [0.07111501 -0.07150972 0 0 0];
fisheye_calib = struct('f', [f, f], 'c', c, 'k', k, 'size', sz, 'fisheye', true);

u_img = make_imageset(d_img);
panel_img_type = 'u';

axes(handles.axes1);
axis off;
u_img_handle = imshow(u_img);
xlabel(' ');
set(u_img_handle,'ButtonDownFcn', @axes1_ButtonDownFcn);
axes(handles.axes3);
axis off;
d_img_handle = imshow(d_img);
set(d_img_handle,'ButtonDownFcn', @axes3_ButtonDownFcn)
% h=findobj(handles.axes3,'type','image');
% set(h,'visible','off')

set(handles.axes3,'visible','off');

set(get(handles.axes3,'children'),'visible','off');


% --- If Enable == 'on', executes on mouse press in 5 pixel border.
% --- Otherwise, executes on mouse press in 5 pixel border or over OpenBtn.
function OpenBtn_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to OpenBtn (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
function axes1_ButtonDownFcn(hObject, eventdata, handles)
global rays_set;
global cp1;
global cp2;
global cp3;
global isClicked;
global mIdx;
isClicked = false;
mIdx =-1;
delete(cp1);
delete(cp2);
delete(cp3);
H =findall(gcf,'Tag','ID');
set(H,'String' , "");
H =findall(gcf,'Tag','face');
set(H,'String' , "");
H =findall(gcf,'Tag','height');
set(H,'String' , "");
H =findall(gcf,'Tag','point1');
set(H,'String' , "");
H =findall(gcf,'Tag','point2');
set(H,'String' , "");
H =findall(gcf,'Tag','point3');
set(H,'String' , "");
% --- Executes on mouse press over axes background.
function cube_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to axes1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% disp('hello world');
global rays_set;
global cp1;
global cp2;
global cp3;
global pfunc;
global pfunc2;
global pfunc3;
global isClicked;
global mIdx;
% if isClicked == true
%      return;
% end
global find_mode;
global isAdd;
global fisheye_calib;
global panel_img_type;
delete(cp1);
delete(cp2);
delete(cp3);

if find_mode || isAdd;
    return;
end

s_point = eventdata.IntersectionPoint;

switch panel_img_type
    case 'd'; [is_find, cube_idx] = find_cube_fisheye(s_point(1:2), rays_set);
    case 'u'; [is_find, cube_idx] = find_cube(s_point(1:2),rays_set);
    otherwise; return;
end

mIdx = cube_idx;

if is_find
    
    isClicked = true;
    pts_cube = get_cube_pts(rays_set{cube_idx}.calib,rays_set{cube_idx}.rays , ...
        rays_set{cube_idx}.type_char,960, 480);
    pts_cube_fisheye = ray2pix(fisheye_calib, rays_set{cube_idx}.rays);
    
    %set txt panel
    H =findall(gcf,'Tag','ID');
    set(H,'String' , string(cube_idx));
    H =findall(gcf,'Tag','face');
    set(H,'String' , string(rays_set{cube_idx}.face));
    H =findall(gcf,'Tag','height');
    set(H,'String' , string(rays_set{cube_idx}.vh));
    H =findall(gcf,'Tag','point1');
    set(H,'String' , "x :" + string(pts_cube_fisheye(1,1)) + newline +"y: "+ string(pts_cube_fisheye(2,1)));
    H =findall(gcf,'Tag','point2');
    set(H,'String' , "x :" +string(pts_cube_fisheye(1,2)) + newline +"y: "+ string(pts_cube_fisheye(2,2)));
    H =findall(gcf,'Tag','point3');
    set(H,'String' , "x :" +string(pts_cube_fisheye(1,3)) + newline +"y: "+string(pts_cube_fisheye(2,3)));
    
    switch panel_img_type
        
        case 'd';
            cp1 = impoint(gca , [pts_cube_fisheye(1,1) pts_cube_fisheye(2,1)]);
            cp2  = impoint(gca, [pts_cube_fisheye(1,2) pts_cube_fisheye(2,2)]);
            cp3  = impoint(gca, [pts_cube_fisheye(1,3) pts_cube_fisheye(2,3)]);
        case 'u';
            cp1 = impoint(gca , [pts_cube(1,1) pts_cube(2,1)]);
            cp2  = impoint(gca, [pts_cube(1,2) pts_cube(2,2)]);
            cp3  = impoint(gca, [pts_cube(1,3) pts_cube(2,3)]);
        otherwise; return;
            
    end
    
    
    setColor(cp1, [1 1 0]); %Red line rectangle
    setColor(cp2, [1 1 0]); %Red line rectangle
    setColor(cp3, [0 1 0]); %Red 
    
    pfunc =  @(p1)(point_callback(1,cube_idx));
    pfunc2 =  @(p2)(point_callback(2,cube_idx));
    pfunc3 =  @(p3)(point_callback(3,cube_idx));
    addNewPositionCallback(cp1,pfunc);
    addNewPositionCallback(cp2,pfunc2);
    addNewPositionCallback(cp3,pfunc3);
end


% --- Executes on button press in add2.

function add2_Callback(hObject, eventdata, handles)
%% 2point click
% hObject    handle to add2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global panel_img_type;
global isAdd;
global rays_set;
global fisheye_calib;
global patch_set_u;
global patch_set_d;
global find_mode;
isAdd = true;

K = [384 0 480 ; 0 384 240 ; 0 0 1]; %normal view intrinsics ..
w = 960; %normal view width
h = 480; %normal view heigt
th = 50*pi/180; %view angle
vh = 1.4; %vehicle height

% xlabel('1. 복사할 큐브를 선택해 주세요.' , 'Color' ,'r');
[in_x , in_y] = myginput(1,'hand');

find_mode = true;

if panel_img_type =='u'
    %% normal view 2point
    [is_find, cube_idx] = find_cube([in_x , in_y],rays_set);
    
    if is_find ~= true
        xlabel(' ','Color' ,'r' );
        isAdd = false;
        find_mode = false;
%         xlabel('cube 를 선택하지 않은것 같습니다.' , 'Color' , 'r');
        return;
    end
    pts_cube = get_cube_pts(rays_set{cube_idx}.calib,rays_set{cube_idx}.rays , ...
        rays_set{cube_idx}.type_char, w, h);
    
%     xlabel('2. 앞 두점을 찍어 주세요...' , 'Color' ,'r');
    [in_x , in_y] = ginput(2);
    xlabel(' ' , 'Color' ,'r');
    pts_tmp = pts_cube(:,2:3);
    find_mode = false;
    
    %check where the points is annotated
    
    i_set = [inpolygon(in_x, in_y, [1 w w 1] , [1 1 h h]) ...
        inpolygon(in_x, in_y, [w 2*w w*2 w] , [1  1 h h]) ...
        inpolygon(in_x, in_y, [1 w w 1] , [h h 2*h 2*h]) ...
        inpolygon(in_x, in_y, [w 2*w 2*w w] , [h h 2*h 2*h])];
    
    [m, view_idx] = max(sum(i_set,1));
    
    if m == 0
%         xlabel('점을 잘못 치신것 같습니다 ...' );
        return;
    end
    
    tmp_obj = make_cube_object(K,[in_x'; in_y'], view_idx, vh , 'u' , rays_set{cube_idx});
    rays_set{end+1} = tmp_obj;
    axes(handles.axes1);
    hold on;
    pts_cube = get_cube_pts(rays_set{end}.calib,rays_set{end}.rays ,view_idx , w, h);
    draw_cube_un(pts_cube , @cube_ButtonDownFcn);
    hold off
    
    axes(handles.axes3);
    
    hold on;
    draw_cube_fisheye(fisheye_calib, rays_set{end}.rays, @cube_ButtonDownFcn);
    hold off;
    set(handles.axes3,'visible','off');
    set(get(handles.axes3,'children'),'visible','off');
    
else
    %% omni_view
    axes(handles.axes3);
    [is_find, cube_idx] = find_cube_fisheye([in_x , in_y], rays_set);
    if is_find ~= true
        xlabel(' ','Color' ,'r' );
        isAdd = false;
        find_mode = false;
%         xlabel('cube 를 선택하지 않은것 같습니다.' , 'Color' , 'r');
        return;
    end
    
%     xlabel('2. 앞 두점을 찍어 주세요...' , 'Color' ,'r');
    [in_x , in_y] = ginput(2);
    xlabel(' ' , 'Color' ,'r');
    
    find_mode = false;
    
    %check where the points is annotated
    rays = pix2ray(fisheye_calib, [in_x'; in_y']);
    
    view_idx = check_un_view(K , rays, w, h ,th);
    tmp_obj = make_cube_object(K,rays, view_idx, vh , 'd',rays_set{cube_idx});
    rays_set{end+1} = tmp_obj;
    axes(handles.axes1);
    hold on;
    
    view_idx = check_un_view(K , rays_set{end}.rays(:,1:4), w, h ,th);
    rays_set{end}.type_char = view_idx;
    pts_cube = get_cube_pts(rays_set{end}.calib,rays_set{end}.rays ,view_idx , w, h);
    draw_cube_un(pts_cube, @cube_ButtonDownFcn);
    hold off
    
    axes(handles.axes3);
    hold on;
    draw_cube_fisheye(fisheye_calib, rays_set{end}.rays , @cube_ButtonDownFcn);
    hold off;
    
    set(handles.axes1,'visible','off');
    set(get(handles.axes1,'children'),'visible','off');
end
isAdd = false;

% --- Executes during object deletion, before destroying properties.
function text2_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to text2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes during object creation, after setting all properties.
function axes1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to axes1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: place code in OpeningFcn to populate axes1


% --- Executes on button press in pushbutton6.
function pushbutton6_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% global rays_set;
% global cp1;
% global cp2;
% global cp3;
% global pfunc;
% global pfunc2;
% global pfunc3;
%
% delete(cp1);
% delete(cp2);
% delete(cp3);
% axes(handles.axes1);
% info_str = " ";
%
% s_point = myginput(1,'hand');
% [is_find, cube_idx] = find_cube(s_point,rays_set);
%
%
% if is_find
%     pts_cube = get_cube_pts(rays_set{cube_idx}.calib,rays_set{cube_idx}.rays , ...
%         rays_set{cube_idx}.type_char, rays_set{cube_idx}.w, rays_set{cube_idx}.h);
%
%     info_str = "idx    : " + string(cube_idx) + newline  + ...
%         "face  : " + string(rays_set{cube_idx}.face) + newline + ...
%         "point1: " + newline + "    " + string(pts_cube(1,1)) + newline + "    "  + string(pts_cube(2,1)) + newline + ...
%         "point2: " + newline + "    " + string(pts_cube(1,2)) + newline + "    "  + string(pts_cube(2,2)) + newline + ...
%         "point3: " + newline + "    " + string(pts_cube(1,3)) + newline + "    "  + string(pts_cube(2,3)) + newline + ...
%         "point5: " + newline + "    " + string(pts_cube(1,5)) + newline + "    "  + string(pts_cube(2,5)) + newline;
%
%     cp1 = impoint(gca , [pts_cube(1,1) pts_cube(2,1)]);
%     cp2  = impoint(gca, [pts_cube(1,2) pts_cube(2,2)]);
%     cp3  = impoint(gca, [pts_cube(1,3) pts_cube(2,3)]);
%
%     setColor(cp1, [1 1 0]); %Red line rectangle
%     setColor(cp2, [1 1 0]); %Red line rectangle
%     setColor(cp3, [1 0 0]); %Red line rectangle
%     pfunc =  @(p1)(point_callback(1,cube_idx));
%
%     pfunc2 =  @(p2)(point_callback(2,cube_idx));
%     pfunc3 =  @(p3)(point_callback(3,cube_idx));
%     addNewPositionCallback(cp1,pfunc);
%     addNewPositionCallback(cp2,pfunc2);
%     addNewPositionCallback(cp3,pfunc3);
% end
% set(handles.text3, 'String', info_str);



% --- Executes on button press in pushbutton7.
function pushbutton7_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes during object creation, after setting all properties.
function text3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to text3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called


% --- Executes on button press in pushbutton8.
function pushbutton8_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global panel_img_type;
global u_img;

if panel_img_type ~= 'u'
    return ;
end
axes(handles.axes1);
xlabel('Select view that you want to see ' , 'Color' ,'r');
s_point = myginput(1,'hand');
w = 960;
h = 480;

in_left = inpolygon(s_point(1), s_point(2), [1 w w 1] , [1 1 h h]);
in_up = inpolygon(s_point(1), s_point(2), [w 2*w w*2 w] , [1  1 h h]);
in_right = inpolygon(s_point(1), s_point(2), [1 w w 1] , [h h 2*h 2*h]);
in_down = inpolygon(s_point(1), s_point(2), [w 2*w 2*w w] , [h h 2*h 2*h]);


if sum(in_left) == 1
    figure(1)
    imshow(u_img(1:h, 1:w));
    
elseif sum(in_up) == 1
    figure(1)
    imshow(u_img(1:h, w:2*w));
    
elseif sum(in_right) == 1
    figure(1)
    imshow(u_img(h:2*h, 1:w));
    
elseif sum(in_down) == 1
    figure(1)
    imshow(u_img(h:2*h, w:2*w));
else
    type_char = ' ';
end

% [in_x , in_y] = ginput(3);


% --- Executes on mouse press over figure background, over a disabled or
% --- inactive control, or over an axes background.
function figure1_WindowButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on mouse press over figure background, over a disabled or
% --- inactive control, or over an axes background.
function figure1_WindowButtonUpFcn(hObject, eventdata, handles)

% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global isM;
global mIdx;
global rays_set;
global fisheye_calib;
global patch_set_d;
global patch_set_u;
global cp3;

if isM ~= true || mIdx  == -1
    return;
end
K = [384 0 480 ; 0 384 240 ; 0 0 1]; %normal view intrinsics ..
w = 960; %normal view width
h = 480; %normal view heigt
th = 50*pi/180; %view angle

axes(handles.axes1);
hold on;
v_idx = check_un_view(K,rays_set{mIdx}.rays(:,1:3) , w, h ,th);
rays_set{mIdx}.type_char = v_idx;
pts_cube = get_cube_pts(rays_set{mIdx}.calib,rays_set{mIdx}.rays ,v_idx, 960, 480);
set(patch_set_u{mIdx}.up,'XData',pts_cube(1,5:end), 'YData',  pts_cube(2,5:end));
set(patch_set_u{mIdx}.down,'XData',pts_cube(1,1:4), 'YData',  pts_cube(2,1:4));
set(patch_set_u{mIdx}.front,'XData', [pts_cube(1, 1:2) pts_cube(1, 6:-1:5)], 'YData', [pts_cube(2, 1:2) pts_cube(2, 6:-1:5)]);
set(patch_set_u{mIdx}.behind,'XData', [pts_cube(1, 3:4) pts_cube(1, 8:-1:7)], 'YData', [pts_cube(2, 3:4) pts_cube(2, 8:-1:7)]);

set(patch_set_u{mIdx}.left,'XData', [pts_cube(1, 1:3:4) pts_cube(1, 8:-3:5)], 'YData', [pts_cube(2, 1:3:4) pts_cube(2, 8:-3:5)]);
set(patch_set_u{mIdx}.right,'XData', [pts_cube(1, 2:3) pts_cube(1, 7:-1:6)], 'YData',  [pts_cube(2, 2:3) pts_cube(2, 7:-1:6)]);
hold off;
axes(handles.axes3);
% axes(handles.axes3);
hold on;
pts_set = get_cube_pts_fisheye(fisheye_calib, rays_set{mIdx}.rays);
set(patch_set_d{mIdx}.front,'XData',pts_set.front(1,:), 'YData',  pts_set.front(2,:));
set(patch_set_d{mIdx}.behind,'XData',pts_set.behind(1,:), 'YData',  pts_set.behind(2,:));
set(patch_set_d{mIdx}.up,'XData',pts_set.up(1,:), 'YData',  pts_set.up(2,:));
set(patch_set_d{mIdx}.left,'XData',pts_set.left(1,:), 'YData',  pts_set.left(2,:));
set(patch_set_d{mIdx}.right,'XData',pts_set.right(1,:), 'YData',  pts_set.right(2,:));
set(patch_set_d{mIdx}.down,'XData',pts_set.down(1,:), 'YData',  pts_set.down(2,:));
hold off;
% cube_idx = mIdx;
% pts_cube = get_cube_pts(rays_set{cube_idx}.calib,rays_set{cube_idx}.rays ,rays_set{cube_idx}.type_char, 960, 480);
% setPosition(cp3, [pts_cube(1,3) pts_cube(2,3)]);
isM = false;




% --- Executes during object creation, after setting all properties.
function axes3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to axes3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: place code in OpeningFcn to populate axes3


% --- Executes on key press with focus on figure1 or any of its controls.
function figure1_WindowKeyPressFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  structure with the following fields (see MATLAB.UI.FIGURE)
%	Key: name of the key that was pressed, in lower case
%	Character: character interpretation of the key(s) that was pressed
%	Modifier: name(s) of the modifier key(s) (i.e., control, shift) pressed
% handles    structure with handles and user data (see GUIDATA)
global isClicked;
global rays_set;
global mIdx;
global patch_set_d;
global patch_set_u;
global copy_obj;
global fisheye_calib;
global cp1;
global cp2;
global cp3;
global panel_img_type;

% disp(mIdx);
if eventdata.Key == 'v' && isfield(copy_obj,'rays') ;
    delete(cp1);
    delete(cp2);
    delete(cp3);
    rays_set{end+1} = copy_obj;
    pts_cube = get_cube_pts(rays_set{end}.calib,rays_set{end}.rays ,rays_set{end}.type_char, 960 , 480);
    axes(handles.axes1);
    hold on;
    draw_cube_un(pts_cube ,@cube_ButtonDownFcn);
    hold off;
    rsz = size(rays_set);
    copy_object(rsz(2));
    axes(handles.axes3);
    hold on;
    draw_cube_fisheye(fisheye_calib, rays_set{end}.rays, @cube_ButtonDownFcn);
    hold off;
    switch panel_img_type
        case 'u';
            set(handles.axes1,'visible','on');
            set(get(handles.axes1,'children'),'visible','on');
            set(handles.axes3,'visible','off');
            set(get(handles.axes3,'children'),'visible','off');
        case 'd';
            set(handles.axes3,'visible','on');
            set(get(handles.axes3,'children'),'visible','on');
            set(handles.axes1,'visible','off');
            set(get(handles.axes1,'children'),'visible','off');
    end
    
end
if isClicked ~= true;
    return;
end

if mIdx == -1;
    return;
end
switch eventdata.Key
    case 'd';
        delete_object(mIdx);
        H =findall(gcf,'Tag','ID');
        set(H,'String' , "");
        H =findall(gcf,'Tag','face');
        set(H,'String' , "");
        H =findall(gcf,'Tag','height');
        set(H,'String' , "");
        H =findall(gcf,'Tag','point1');
        set(H,'String' , "");
        H =findall(gcf,'Tag','point2');
        set(H,'String' , "");
        H =findall(gcf,'Tag','point3');
        set(H,'String' , "");
    case 'c';
        disp('copyed');
        copy_object(mIdx);
        
    case '1';
        %change height to 1
        height_change(mIdx , 1);
        H =findall(gcf,'Tag','height');
        set(H,'String' , string(rays_set{mIdx}.vh));
        
    case '2';
        %change height to 2
        height_change(mIdx , 2);
        H =findall(gcf,'Tag','height');
        set(H,'String' , string(rays_set{mIdx}.vh));
        
    case '3';
        %change height to 2
        height_change(mIdx , 3);
        H =findall(gcf,'Tag','height');
        set(H,'String' , string(rays_set{mIdx}.vh));
    case 's';
        rays_set{mIdx}.face = 2;
        H =findall(gcf,'Tag','face');
        set(H,'String' , string(rays_set{mIdx}.face));
           
         set_patches(rays_set{mIdx}.face , mIdx)
    case 'f';
        rays_set{mIdx}.face = 1;
        H =findall(gcf,'Tag','face');
        set(H,'String' , string(rays_set{mIdx}.face));
        set_patches(rays_set{mIdx}.face , mIdx)
%         set_patches('FaceColor' ,'red')
    case 'b';
        rays_set{mIdx}.face = 2;
        H =findall(gcf,'Tag','face');
        set(H,'String' , string(rays_set{mIdx}.face));
        set_patches(rays_set{mIdx}.face ,mIdx)
%         set_patches('FaceColor' ,'blue')
    case 'l';
        rays_set{mIdx}.face = 3;
        H =findall(gcf,'Tag','face');
        set(H,'String' , string(rays_set{mIdx}.face));
        set_patches(rays_set{mIdx}.face ,mIdx)
    case 'r';
        rays_set{mIdx}.face = 4;
        H =findall(gcf,'Tag','face');
        set(H,'String' , string(rays_set{mIdx}.face));
        set_patches(rays_set{mIdx}.face ,mIdx)
end


% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1


% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in loadbutton.
function loadbutton_Callback(hObject, eventdata, handles)
% hObject    handle to loadbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[file_name pathname] = uigetfile({'*.csv','All Image Files';...
    '*.*','All Files' },'File Selector');

full_path = strcat(pathname, file_name);

if exist(full_path ,'file') ~=2
    return;
end
load_csv(full_path,handles,@cube_ButtonDownFcn);



% --- Executes on mouse press over axes background.
function axes3_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to axes3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global rays_set;
global cp1;
global cp2;
global cp3;
global isClicked;
global mIdx;
isClicked = false;
mIdx =-1;
delete(cp1);
delete(cp2);
delete(cp3);
H =findall(gcf,'Tag','ID');
set(H,'String' , "");
H =findall(gcf,'Tag','face');
set(H,'String' , "");
H =findall(gcf,'Tag','height');
set(H,'String' , "");
H =findall(gcf,'Tag','point1');
set(H,'String' , "");
H =findall(gcf,'Tag','point2');
set(H,'String' , "");
H =findall(gcf,'Tag','point3');
set(H,'String' , "");


% --- Executes on button press in prev.
function prev_Callback(hObject, eventdata, handles)
% hObject    handle to prev (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global file_store;
global file_idx;
global file_name;

if file_idx == -1
    return;
end
file_name = file_store.Files{file_idx};
answer = questdlg('저장 하시겠습니까?' , 'Select'...
                    , 'yes' , 'no', 'cancel','cancel');
f_sz = size(file_store.Files);
prev_idx =  mod(file_idx-1, f_sz(1)+1);
if prev_idx == 0
    prev_idx = f_sz(1);
end
switch answer
    case 'yes';
        save_csv(handles);
%         delete_all();
        file_name = file_store.Files{prev_idx};
        image_open(handles, file_store.Files{prev_idx},@axes1_ButtonDownFcn,@axes3_ButtonDownFcn);
    case 'no';
        
%         delete_all();
        file_name = file_store.Files{prev_idx};
        image_open(handles, file_store.Files{prev_idx},@axes1_ButtonDownFcn,@axes3_ButtonDownFcn);
    case 'cancel';
        return ;
end

% --- Executes on button press in next.
function next_Callback(hObject, eventdata, handles)
% hObject    handle to next (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global file_store;
global file_idx;
global file_name;
 

if file_idx == -1
    return;
end
answer = questdlg('저장 하시겠습니까?' , 'Select'...
                    , 'yes' , 'no', 'cancel','cancel');
f_sz = size(file_store.Files);
next_idx =  mod(file_idx+1, f_sz(1)+1);

if next_idx == 0
    next_idx = 1;
end
switch answer
    case 'yes';
        save_csv(handles);
%         delete_all();
        file_name = file_store.Files{next_idx};
        image_open(handles, file_store.Files{next_idx},@axes1_ButtonDownFcn,@axes3_ButtonDownFcn);
    case 'no';
%         delete_all();
        file_name = file_store.Files{next_idx};
        image_open(handles, file_store.Files{next_idx},@axes1_ButtonDownFcn,@axes3_ButtonDownFcn);
    case 'cancel';
        return ;
end
        


% --- Executes on button press in pushbutton12.
function pushbutton12_Callback(hObject, eventdata, handles)

% hObject    handle to pushbutton12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
delete_all();
