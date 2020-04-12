function pts_cube = make_3d_cubes(calib,K, pts ,type,vh, h ,w )

% fx = calib.f(1);  fy = calib.f(2);
% cx = calib.c(1);  cy = calib.c(2);

% K = [fx, 0, cx; 0, fy, cy; 0, 0, 1];

th = 50 * pi / 180;
pts(3,:) = 1;
% make ray
rays = K\pts;

switch type
    case 'u'; rays = (rodrigues([th, 0, 0])) *rays;
    case 'd'; rays = (rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]))* rays;
    case 'r'; rays = (rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2])) * rays;
    case 'l'; rays = rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2]) * rays;
end

% assume the points annotated by human are on the plane (z = 3)
rays = rays./rays(3,:);
rays = rays*3;
rays(:,end+1) = rays(:,3)-rays(:,2)+rays(:,1);

%calculate cube
rays_cube = repmat(rays , [1 2]);
rays_cube(3, 5:end) = 3-vh;
rays_cube = rays_cube./rays_cube(3,:);

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
lx_76 = linspace(rays_cube(1,3) , rays_cube(1,7),10);
ly_76 = linspace(rays_cube(2,3) , rays_cube(2,7),10);
lx_62 = linspace(rays_cube(1,6) , rays_cube(1,2),10);
ly_62 = linspace(rays_cube(2,6) , rays_cube(2,2),10);

%left
lx_14 = linspace(rays_cube(1,1) , rays_cube(1,1),10);
ly_14 = linspace(rays_cube(2,1) , rays_cube(2,1),10);
lx_48 = linspace(rays_cube(1,4) , rays_cube(1,4),10);
ly_48 = linspace(rays_cube(2,4) , rays_cube(2,4),10);
% lx_85 = linspace(rays_cube(1,8) , rays_cube(1,8),10);
% ly_85 = linspace(rays_cube(2,8) , rays_cube(2,8),10);
% lx_51 = linspace(rays_cube(1,5) , rays_cube(1,5),10);
% ly_51 = linspace(rays_cube(2,5) , rays_cube(2,5),10);

%up
lx_56 = linspace(rays_cube(1,5) , rays_cube(1,8),10);
ly_56 = linspace(rays_cube(2,5) , rays_cube(2,8),10);
lx_67 = linspace(rays_cube(1,6) , rays_cube(1,6),10);
ly_67 = linspace(rays_cube(2,6) , rays_cube(2,6),10);
lx_78 = linspace(rays_cube(1,7) , rays_cube(1,7),10);
ly_78 = linspace(rays_cube(2,7) , rays_cube(2,7),10);
lx_85 = linspace(rays_cube(1,8) , rays_cube(1,8),10);
ly_85 = linspace(rays_cube(2,8) , rays_cube(2,8),10);

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

figure(1);
hold on


%% draw on the distorted image
pts_front = ray2pix(calib, rays_front);
pts_behind = ray2pix(calib, rays_behind);
pts_up = ray2pix(calib, rays_up);
pts_down = ray2pix(calib, rays_down);
pts_left = ray2pix(calib, rays_left);
pts_right = ray2pix(calib, rays_right);


%up, down
fill(pts_up(1,:), pts_up(2,:), 'blue', 'FaceAlpha',0.3);
fill(pts_down(1,:), pts_down(2,:), 'blue', 'FaceAlpha',0.3);
% %front, behind
fill(pts_front(1,:) , pts_front(2,:),'green' , 'FaceAlpha', 0.3);
fill(pts_behind(1,:) , pts_behind(2,:),'green' , 'FaceAlpha', 0.3);
% %left ,right
fill(pts_left(1,:) , pts_left(2,:),'red' , 'FaceAlpha', 0.3);
fill(pts_right(1,:) , pts_right(2,:),'red' , 'FaceAlpha', 0.3);

%% draw cube on the undistorted image
switch type
    case 'u'; rays_cube = rodrigues([th, 0, 0]) \ rays_cube;
    case 'd'; rays_cube = rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]) \ rays_cube;
    case 'r'; rays_cube = rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2]) \ rays_cube;
    case 'l'; rays_cube = (rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2])) \ rays_cube;
end

rays_cube = rays_cube./rays_cube(3,:);
pts_cube = K*rays_cube;
switch type
    case 'u'; pts_cube(1,:) = pts_cube(1,:) + w;
    case 'd'; pts_cube(1,:) = pts_cube(1,:) + w; pts_cube(2,:) = pts_cube(2,:) + h;
    case 'r'; pts_cube(2,:) = pts_cube(2,:) + h;
end
figure(2)
%up, down
fill(pts_cube(1,5:end), pts_cube(2,5:end),'blue', 'FaceAlpha', 0.1);
fill(pts_cube(1,1:4), pts_cube(2,1:4),'blue', 'FaceAlpha', 0.1);
%front, behind
fill([pts_cube(1, 1:2) pts_cube(1, 6:-1:5)], [pts_cube(2, 1:2) pts_cube(2, 6:-1:5)] ,'green' ,'FaceAlpha', 0.1);
fill([pts_cube(1, 3:4) pts_cube(1, 8:-1:7)], [pts_cube(2, 3:4) pts_cube(2, 8:-1:7)] ,'green' ,'FaceAlpha', 0.1);
%left ,right
fill([pts_cube(1, 1:3:4) pts_cube(1, 8:-3:5)], [pts_cube(2, 1:3:4) pts_cube(2, 8:-3:5)] ,'red' ,'FaceAlpha', 0.1);
fill([pts_cube(1, 2:3) pts_cube(1, 7:-1:6)], [pts_cube(2, 2:3) pts_cube(2, 7:-1:6)] ,'red' ,'FaceAlpha', 0.1);



end