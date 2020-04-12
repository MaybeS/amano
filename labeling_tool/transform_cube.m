function rays_cube = transform_cube(type, K, point_idx, change_point, rays_cube ,vh)

global fisheye_calib;
th = 50 * pi / 180;
tmp = rays_cube;
change_point(3,:) = 1;

if ~isfield(K, 'k')
        new_ray  =  K\change_point;
        switch type
            case 2; new_ray = (rodrigues([th, 0, 0])) *new_ray;
            case 4; new_ray = (rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]))* new_ray;
            case 3; new_ray = (rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2])) * new_ray;
            case 1; new_ray = rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2]) * new_ray;
        end
        
        
else
        new_ray = pix2ray(fisheye_calib,change_point(1:2,:));
        
end
new_ray = new_ray./new_ray(3,:);
new_ray = new_ray*3;
rays_cube(:,1:4) = rays_cube(:,1:4)*3;
switch point_idx
    case 1;
        v21 = rays_cube(:,1) - rays_cube(:,2);
        v21 = v21/norm(v21);
        
        v21n = new_ray - rays_cube(:,2);
        v21n = v21n/norm(v21n);
        
        v23 = rays_cube(:,3) - rays_cube(:,2);
        
        v = cross(v21, v21n);
        ssc = [0 -v(3) v(2) ; v(3) 0 -v(1); -v(2) v(1) 0];
        R = eye(3,3) + ssc + ssc*ssc*(1- dot(v21, v21n))/ (norm(v)*norm(v));
        if sum(sum(isnan((R)))) >= 1
            rays_cube = tmp;
            return;
        end
        
        v23 = R*v23;
        
        rays_cube(:, 1) = new_ray;
        rays_cube(:, 3) = rays_cube(:,2)+ v23;
        rays_cube(:,4) = rays_cube(:,3)-rays_cube(:,2)+rays_cube(:,1);
        
    case 2;
        %                 if norm(new_ray -rays_cube(:,1))<1
        %                     return;
        %                 end
        v12 = rays_cube(:,2) - rays_cube(:,1);
        v12 = v12/norm(v12);
        
        v12n = new_ray - rays_cube(:,1);
        v12n = v12n/norm(v12n);
        
        v14 = rays_cube(:,4) - rays_cube(:,1);
        v = cross(v12, v12n);
        ssc = [0 -v(3) v(2) ; v(3) 0 -v(1); -v(2) v(1) 0];
        R = eye(3,3) + ssc + ssc*ssc*(1- dot(v12, v12n))/ (norm(v)*norm(v));
        
        if sum(sum(isnan((R)))) >= 1
            rays_cube = tmp;
            
            return ;
        end
        
        v14 = R*v14;
        rays_cube(:, 2) = new_ray;
        %         rays_cube(1:2,3) = rays_cube(1:2,2) + ortho_vec./norm(ortho_vec)* BC_norm;
        %         rays_cube(:, 4) = rays_cube(:,1)+ rays_cube(:,3) - rays_cube(:,2);
        rays_cube(:,4) = rays_cube(:,1) + v14;
        rays_cube(:,3) = rays_cube(:,4)-rays_cube(:,1)+rays_cube(:,2);
    case 3;
        
        n = norm(rays_cube(1:2,3)- rays_cube(1:2,2));
        rays_cube(1:2,3) = rays_cube(1:2,2) + (rays_cube(1:2,3) - rays_cube(1:2,2))/n*norm(new_ray(1:2)- rays_cube(1:2,2));
        rays_cube(:,4) = rays_cube(:,3)- rays_cube(:,2) + rays_cube(:,1);
end

rays_cube = repmat(rays_cube(:,1:4) , [1 2]);
rays_cube(3, 5:end) = 3-vh;
rays_cube = rays_cube./rays_cube(3,:);
end