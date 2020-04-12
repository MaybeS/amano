function height_change(cube_idx , height_idx)
global rays_set;
global patch_set_u;
global patch_set_d;
global fisheye_calib;
vh = 1.4;
switch height_idx
    case 1; vh = 1.4;
    case 2; vh = 1.7;
    case 3; vh = 2.0;
end

tmp_ray = rays_set{cube_idx}.rays;
tmp_ray = tmp_ray*3;
tmp_ray = repmat(tmp_ray(:,1:4) ,[1 2]);
tmp_ray(3,5:end) = 3-vh;
tmp_ray= tmp_ray./tmp_ray(3,:);
rays_set{cube_idx}.rays = tmp_ray;
rays_set{cube_idx}.vh = vh;
pts_cube = get_cube_pts(rays_set{cube_idx}.calib,rays_set{cube_idx}.rays ,rays_set{cube_idx}.type_char,960, 480);

%redraw patch
set(patch_set_u{cube_idx}.up,'XData',pts_cube(1,5:end), 'YData',  pts_cube(2,5:end));
set(patch_set_u{cube_idx}.down,'XData',pts_cube(1,1:4), 'YData',  pts_cube(2,1:4));
set(patch_set_u{cube_idx}.front,'XData', [pts_cube(1, 1:2) pts_cube(1, 6:-1:5)], 'YData', [pts_cube(2, 1:2) pts_cube(2, 6:-1:5)]);
set(patch_set_u{cube_idx}.behind,'XData', [pts_cube(1, 3:4) pts_cube(1, 8:-1:7)], 'YData', [pts_cube(2, 3:4) pts_cube(2, 8:-1:7)]);
set(patch_set_u{cube_idx}.left,'XData', [pts_cube(1, 1:3:4) pts_cube(1, 8:-3:5)], 'YData', [pts_cube(2, 1:3:4) pts_cube(2, 8:-3:5)]);
set(patch_set_u{cube_idx}.right,'XData', [pts_cube(1, 2:3) pts_cube(1, 7:-1:6)], 'YData',  [pts_cube(2, 2:3) pts_cube(2, 7:-1:6)]);

pts_set = get_cube_pts_fisheye(fisheye_calib, rays_set{cube_idx}.rays);
set(patch_set_d{cube_idx}.front,'XData',pts_set.front(1,:), 'YData',  pts_set.front(2,:));
set(patch_set_d{cube_idx}.behind,'XData',pts_set.behind(1,:), 'YData',  pts_set.behind(2,:));
set(patch_set_d{cube_idx}.up,'XData',pts_set.up(1,:), 'YData',  pts_set.up(2,:));
set(patch_set_d{cube_idx}.left,'XData',pts_set.left(1,:), 'YData',  pts_set.left(2,:));
set(patch_set_d{cube_idx}.right,'XData',pts_set.right(1,:), 'YData',  pts_set.right(2,:));
set(patch_set_d{cube_idx}.down,'XData',pts_set.down(1,:), 'YData',  pts_set.down(2,:));
end