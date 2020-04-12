function draw_cube_fisheye(calib ,rays_cube , cube_func)
global patch_set_d;
%front
% lx_12 = linspace(rays_cube(1,1) , rays_cube(1,2),10);
% ly_12 = linspace(rays_cube(2,1) , rays_cube(2,2),10);
lx_26 = linspace(rays_cube(1,2) , rays_cube(1,6),10);
ly_26 = linspace(rays_cube(2,2) , rays_cube(2,6),10);
lx_65 = linspace(rays_cube(1,6) , rays_cube(1,5),10);
ly_65 = linspace(rays_cube(2,6) , rays_cube(2,5),10);
lx_51 = linspace(rays_cube(1,5) , rays_cube(1,1),10);
ly_51 = linspace(rays_cube(2,5) , rays_cube(2,1),10);

%behind 
lx_43 = linspace(rays_cube(1,4) , rays_cube(1,3),10);
ly_43 = linspace(rays_cube(2,4) , rays_cube(2,3),10);
% lx_37 = linspace(rays_cube(1,3) , rays_cube(1,7),10);
% ly_37 = linspace(rays_cube(2,3) , rays_cube(2,7),10);
% lx_78 = linspace(rays_cube(1,7) , rays_cube(1,8),10);
% ly_78 = linspace(rays_cube(2,7) , rays_cube(2,8),10);
lx_84 = linspace(rays_cube(1,8) , rays_cube(1,4),10);
ly_84 = linspace(rays_cube(2,8) , rays_cube(2,4),10);

%right
% lx_23 = linspace(rays_cube(1,2) , rays_cube(1,3),10);
% ly_23 = linspace(rays_cube(2,2) , rays_cube(2,3),10);
lx_37 = linspace(rays_cube(1,3) , rays_cube(1,7),10);
ly_37 = linspace(rays_cube(2,3) , rays_cube(2,7),10);
lx_76 = linspace(rays_cube(1,7) , rays_cube(1,6),10);
ly_76 = linspace(rays_cube(2,7) , rays_cube(2,6),10);
lx_62 = linspace(rays_cube(1,6) , rays_cube(1,2),10);
ly_62 = linspace(rays_cube(2,6) , rays_cube(2,2),10);

%left
lx_14 = linspace(rays_cube(1,1) , rays_cube(1,4),10);
ly_14 = linspace(rays_cube(2,1) , rays_cube(2,4),10);
lx_48 = linspace(rays_cube(1,4) , rays_cube(1,8),10);
ly_48 = linspace(rays_cube(2,4) , rays_cube(2,8),10);
% lx_85 = linspace(rays_cube(1,8) , rays_cube(1,8),10);
% ly_85 = linspace(rays_cube(2,8) , rays_cube(2,8),10);
% lx_51 = linspace(rays_cube(1,5) , rays_cube(1,5),10);
% ly_51 = linspace(rays_cube(2,5) , rays_cube(2,5),10);

%up
lx_56 = linspace(rays_cube(1,5) , rays_cube(1,6),10);
ly_56 = linspace(rays_cube(2,5) , rays_cube(2,6),10);
lx_67 = linspace(rays_cube(1,6) , rays_cube(1,7),10);
ly_67 = linspace(rays_cube(2,6) , rays_cube(2,7),10);
lx_78 = linspace(rays_cube(1,7) , rays_cube(1,8),10);
ly_78 = linspace(rays_cube(2,7) , rays_cube(2,8),10);
lx_85 = linspace(rays_cube(1,8) , rays_cube(1,5),10);
ly_85 = linspace(rays_cube(2,8) , rays_cube(2,5),10);

%down 
lx_12 = linspace(rays_cube(1,1) , rays_cube(1,2),10);
ly_12 = linspace(rays_cube(2,1) , rays_cube(2,2),10);
lx_23 = linspace(rays_cube(1,2) , rays_cube(1,3),10);
ly_23 = linspace(rays_cube(2,2) , rays_cube(2,3),10);
lx_34 = linspace(rays_cube(1,3) , rays_cube(1,4),10);
ly_34 = linspace(rays_cube(2,3) , rays_cube(2,4),10);
lx_41 = linspace(rays_cube(1,4) , rays_cube(1,1),10);
ly_41 = linspace(rays_cube(2,4) , rays_cube(2,1),10);

rays_front = [lx_12 lx_26 lx_65 lx_51; ly_12 ly_26 ly_65 ly_51; linspace(1,1,40)];
rays_behind = [lx_43 lx_37 lx_78 lx_84; ly_43 ly_37 ly_78 ly_84; linspace(1,1,40)];
rays_up = [lx_56 lx_67 lx_78 lx_85; ly_56 ly_67 ly_78 ly_85; linspace(1,1,40)];
rays_down = [lx_12 lx_23 lx_34 lx_41; ly_12 ly_23 ly_34 ly_41; linspace(1,1,40)];
rays_left = [lx_14 lx_48 lx_85 lx_51; ly_14 ly_48 ly_85 ly_51; linspace(1,1,40)];
rays_right = [lx_23 lx_37 lx_76 lx_62; ly_23 ly_37 ly_76 ly_62; linspace(1,1,40)];

pts_front = ray2pix(calib, rays_front);
pts_behind = ray2pix(calib, rays_behind);
pts_up = ray2pix(calib, rays_up);
pts_down = ray2pix(calib, rays_down);
pts_left = ray2pix(calib, rays_left);
pts_right = ray2pix(calib, rays_right);


dset = {};
%up, down
dset.up = fill(pts_up(1,:), pts_up(2,:), 'white', 'FaceAlpha',0.1,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
dset.down = fill(pts_down(1,:), pts_down(2,:), 'white', 'FaceAlpha',0.1,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
% %front, behind
dset.front = fill(pts_front(1,:) , pts_front(2,:),'red' , 'FaceAlpha', 0.5,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
dset.behind = fill(pts_behind(1,:) , pts_behind(2,:),'blue' , 'FaceAlpha', 0.5,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
% %left ,right
dset.left = fill(pts_left(1,:) , pts_left(2,:),'green' , 'FaceAlpha', 0.1,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
dset.right = fill(pts_right(1,:) , pts_right(2,:),'green' , 'FaceAlpha', 0.1,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);

patch_set_d{end+1} = dset;
set(patch_set_d{end}.up,'ButtonDownFcn', cube_func);
set(patch_set_d{end}.down,'ButtonDownFcn', cube_func);
set(patch_set_d{end}.left,'ButtonDownFcn', cube_func);
set(patch_set_d{end}.right,'ButtonDownFcn', cube_func);
set(patch_set_d{end}.front,'ButtonDownFcn', cube_func);
set(patch_set_d{end}.behind,'ButtonDownFcn', cube_func);

end
