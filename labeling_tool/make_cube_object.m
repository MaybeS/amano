function obj = make_cube_object(K, rays, view_idx, vh, img_type , copy_obj)
obj ={};
obj.vh = vh;
if ~isfield(copy_obj , 'rays')
    obj.rays = make_cube_rays(K, rays ,view_idx ,obj.vh, img_type);
else
    obj.rays = make_cube_rays2(K, rays ,view_idx ,obj.vh, img_type , copy_obj.rays);
end

obj.type_char = view_idx;
obj.calib = K;
obj.face = 1;
end

function rays_cube = make_cube_rays(K, pts ,type,vh , img_type)

w = 960;
h = 480;

if(img_type  == 'u')
    switch type
        case 2; pts(1,:) = pts(1,:) -w; %up view
        case 3; pts(2,:) = pts(2,:) -h; %down view
        case 4; pts(1,:) = pts(1,:) -w; pts(2,:) = pts(2,:) -h;
    end
    
    th = 50 * pi / 180;
    pts(3,:) = 1;
    % make ray
    rays = K\pts;
    
    switch type
        case 2; rays = (rodrigues([th, 0, 0])) *rays;
        case 4; rays = (rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]))* rays;
        case 3; rays = (rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2])) * rays;
        case 1; rays = rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2]) * rays;
    end
else
    %pts is rays if you annotate point in omniview
    rays = pts;
end
% assume the points annotated by human are on the plane (z = 3)
rays = rays./rays(3,:);
rays = rays*3;

A = rays(1:2,1);
B = rays(1:2,2) - A;
C = rays(1:2,3) - A;

BC_norm = norm(C-B);
ortho_vec = [B(2); -B(1)];

%>90
if dot(ortho_vec , C) <= 0
    ortho_vec = -ortho_vec;
end
rays(1:2,3) = rays(1:2,2) + ortho_vec./norm(ortho_vec)* BC_norm;
rays(:,end+1) = rays(:,3)-rays(:,2)+rays(:,1);

%calculate cube
rays_cube = repmat(rays , [1 2]);
rays_cube(3, 5:end) = 3-vh;
rays_cube = rays_cube./rays_cube(3,:);

end

function rays_cube = make_cube_rays2(K, pts ,type,vh , img_type, copy_ray)
w = 960;
h = 480;
if(img_type  == 'u')
    switch type
        case 2; pts(1,:) = pts(1,:) -w; %up view
        case 3; pts(2,:) = pts(2,:) -h; %down view
        case 4; pts(1,:) = pts(1,:) -w; pts(2,:) = pts(2,:) -h;
    end
    
    th = 50 * pi / 180;
    pts(3,:) = 1;
    % make ray
    rays = K\pts;
    
    switch type
        case 2; rays = (rodrigues([th, 0, 0])) *rays;
        case 4; rays = (rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]))* rays;
        case 3; rays = (rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2])) * rays;
        case 1; rays = rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2]) * rays;
    end
    
else
    %pts is rays if you annotate point in omniview
    rays = pts;
end


%rotation
rays = rays./rays(3,:);
rays = rays*3;
copy_ray = copy_ray./copy_ray(3,:);
copy_ray =copy_ray*3;

v12 = copy_ray(:,2) - copy_ray(:,1);
v12 = v12/norm(v12);

v12n = rays(:,2) - rays(:,1);
v12n = v12n/norm(v12n);

v14 = copy_ray(:,4) - copy_ray(:,1);
v = cross(v12, v12n);
ssc = [0 -v(3) v(2) ; v(3) 0 -v(1); -v(2) v(1) 0];
R = eye(3,3) + ssc + ssc*ssc*(1- dot(v12, v12n))/ (norm(v)*norm(v));

v14 = R*v14;
if dot(copy_ray(:,1) - copy_ray(:,2) , rays(:,1) - rays(:,2)) < 0
    v14 = -v14;
end

rays(:,4) = rays(:,1) + v14;
rays(:,3) = rays(:,4)-rays(:,1)+rays(:,2);

rays_cube = repmat(rays , [1 2]);
rays_cube(3, 5:end) = 3-vh;
rays_cube = rays_cube./rays_cube(3,:);
end