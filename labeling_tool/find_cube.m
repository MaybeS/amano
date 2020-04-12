function [is_find, cube_idx] = find_cube(pt, rays_set)
r_sz = size(rays_set);
is_find = false;
cube_idx = -1;
w= 960;
h= 480;
for i = 1 : r_sz(2)
    
    pts_cube = get_cube_pts(rays_set{i}.calib,rays_set{i}.rays ,rays_set{i}.type_char, w, h);
    min_x = min(pts_cube(1,:));
    max_x = max(pts_cube(1,:));
    
    min_y = min(pts_cube(2,:));
    max_y = max(pts_cube(2,:));
    
    is_inner = inpolygon(pt(1), pt(2), [min_x max_x] , [min_y max_y]);
    
    if is_inner
        is_find = true;
        cube_idx = i;
        break;
    end
    
end

end