function ret = image_open(handles , img_path ,axes1_Btnfunc, axes3_Btnfunc )
global d_img;
global u_img;
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
global u_img_handle;
global d_img_handle;
isAdd = false;
find_mode =false;
% mIdx = -1;
% rays_set = {};
% patch_set_u = {};
% patch_set_d = {};
isClicked = false;
isMove =false;
file_idx = find(strcmp(file_store.Files,img_path));
% [filepath,file_name,ext] = fileparts(file_store.Files{file_idx});
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
set(handles.text2, 'String', img_path);
d_img = imread(img_path);

%calibration param initialize
I = mean(single(d_img), 3) / 255;
sz = fliplr(size(I));
f = 0.26 * sz(1);  % Fotec lens
c = sz / 2 + [77, 33];
k = [0.07111501 -0.07150972 0 0 0];
fisheye_calib = struct('f', [f, f], 'c', c, 'k', k, 'size', sz, 'fisheye', true);

u_img = make_imageset(d_img);
% panel_img_type = 'u';

axes(handles.axes1);
axis off;
%uh = imshow(u_img);
u_img_handle.CData = u_img;
xlabel(' ');
% set(uh,'ButtonDownFcn', axes1_Btnfunc);
axes(handles.axes3);
axis off;
% dh = imshow(d_img);
d_img_handle.CData = d_img;
% set(dh,'ButtonDownFcn', axes3_Btnfunc)
% h=findobj(handles.axes3,'type','image');
% set(h,'visible','off')

if panel_img_type == 'd'
    set(handles.axes1,'visible','off');
    set(get(handles.axes1,'children'),'visible','off');
    set(handles.axes3,'visible','on');
    set(get(handles.axes3,'children'),'visible','on');
else 
    set(handles.axes3,'visible','off');
    set(get(handles.axes3,'children'),'visible','off');
     set(handles.axes1,'visible','on');
    set(get(handles.axes1,'children'),'visible','on');
end
direc_sz  = size(file_store.Files);
dstr =  file_idx+"/"+direc_sz;
set(handles.text17, 'String' , dstr);
end