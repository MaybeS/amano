function load_csv(full_path ,handles , cube_func)
global rays_set;
global panel_img_type;
global fisheye_calib;

delete_all();

w = 960;
h = 480;

m = csvread(full_path);
%frame number, id, face, height, x,y, x, y, x, y , side ,type
% 15

csv_sz = size(m);
type_char = -1;
calib = [384 0 480 ; 0 384 240 ; 0 0 1];
if(m(1,:) == 0 ) 
    return;
end
for i = 1 : csv_sz
    obj ={};
    rays = reshape(m(i,5:10), [2 3]);
    rays = pix2ray(fisheye_calib,rays);
    rays = rays*3;
    rays(:,end+1) = rays(:,3)-rays(:,2)+rays(:,1);
    rays_cube = repmat(rays , [1 2]);
    rays_cube(3, 5:end) = 3-m(i,4);
    rays_cube = rays_cube./rays_cube(3,:);
    
    
    obj.rays = rays_cube;
    obj.type_char = m(i,11);
    obj.calib = calib;
    obj.w = w;
    obj.h = h;
    obj.vh = m(i,4);
    obj.face = m(i,3);
    
    rays_set{end+1} = obj;
    
    axes(handles.axes1);
    xlabel('loaded');
    hold on;
    
    pts_cube = get_cube_pts(rays_set{end}.calib,rays_set{end}.rays , ...
                rays_set{end}.type_char, rays_set{end}.w, rays_set{end}.h);
    draw_cube_un(pts_cube , cube_func);
    hold off;
    
    axes(handles.axes3);
    hold on;
    draw_cube_fisheye(fisheye_calib, rays_set{end}.rays, cube_func);
    hold off;
    
    if panel_img_type == 'u'
    set(handles.axes3,'visible','off');
    set(get(handles.axes3,'children'),'visible','off');
    else
    set(handles.axes1,'visible','off');
    set(get(handles.axes1,'children'),'visible','off');
    end
    set_patches(rays_set{i}.face ,i)
    
end
