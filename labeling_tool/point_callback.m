function point_callback(point_idx,cube_idx)
global rays_set;
global patch_set_u;
global patch_set_d;
global cp1;
global cp2;
global cp3;
global isM;
global mIdx;
global panel_img_type;
global fisheye_calib;
if panel_img_type == 'u'
    calib = [384 0 480 ; 0 384 240 ; 0 0 1];
    
    switch point_idx
        case 1; p = getPosition(cp1);
        case 2; p = getPosition(cp2);
        case 3; p = getPosition(cp3);
    end
    
    
    switch rays_set{cube_idx}.type_char
        case 2; p(1) = p(1) - 960;
        case 3; p(2) = p(2) - 480;
        case 4; p(:) = p(:) - [960; 480];
    end
    rays_cube_new = transform_cube(rays_set{cube_idx}.type_char, calib, point_idx, [p(1) p(2)]' , rays_set{cube_idx}.rays, rays_set{cube_idx}.vh);
    rays_set{cube_idx}.rays= rays_cube_new;
    isM = true;
    mIdx = cube_idx;
    
    pts_cube = get_cube_pts(rays_set{cube_idx}.calib,rays_set{cube_idx}.rays ,rays_set{cube_idx}.type_char, 960, 480);
    
    
    %redraw patch
    set(patch_set_u{cube_idx}.up,'XData',pts_cube(1,5:end), 'YData',  pts_cube(2,5:end));
    set(patch_set_u{cube_idx}.down,'XData',pts_cube(1,1:4), 'YData',  pts_cube(2,1:4));
    set(patch_set_u{cube_idx}.front,'XData', [pts_cube(1, 1:2) pts_cube(1, 6:-1:5)], 'YData', [pts_cube(2, 1:2) pts_cube(2, 6:-1:5)]);
    set(patch_set_u{cube_idx}.behind,'XData', [pts_cube(1, 3:4) pts_cube(1, 8:-1:7)], 'YData', [pts_cube(2, 3:4) pts_cube(2, 8:-1:7)]);
    
    set(patch_set_u{cube_idx}.left,'XData', [pts_cube(1, 1:3:4) pts_cube(1, 8:-3:5)], 'YData', [pts_cube(2, 1:3:4) pts_cube(2, 8:-3:5)]);
    set(patch_set_u{cube_idx}.right,'XData', [pts_cube(1, 2:3) pts_cube(1, 7:-1:6)], 'YData',  [pts_cube(2, 2:3) pts_cube(2, 7:-1:6)]);
    hold off;
    setPosition(cp3, [pts_cube(1,3) pts_cube(2,3)]);
 
elseif panel_img_type == 'd'
    
    switch point_idx
        case 1; p = getPosition(cp1);
        case 2; p = getPosition(cp2);
        case 3; p = getPosition(cp3);
    end
    rays_cube_new = transform_cube(rays_set{cube_idx}.type_char, fisheye_calib, point_idx, [p(1) p(2)]' , rays_set{cube_idx}.rays, rays_set{cube_idx}.vh);
    rays_set{cube_idx}.rays= rays_cube_new;
    isM = true;
    mIdx = cube_idx;
    
    hold on;
    pts_set = get_cube_pts_fisheye(fisheye_calib, rays_set{cube_idx}.rays);
    set(patch_set_d{mIdx}.front,'XData',pts_set.front(1,:), 'YData',  pts_set.front(2,:));
    set(patch_set_d{mIdx}.behind,'XData',pts_set.behind(1,:), 'YData',  pts_set.behind(2,:));
    set(patch_set_d{mIdx}.up,'XData',pts_set.up(1,:), 'YData',  pts_set.up(2,:));
    set(patch_set_d{mIdx}.left,'XData',pts_set.left(1,:), 'YData',  pts_set.left(2,:));
    set(patch_set_d{mIdx}.right,'XData',pts_set.right(1,:), 'YData',  pts_set.right(2,:));
    set(patch_set_d{mIdx}.down,'XData',pts_set.down(1,:), 'YData',  pts_set.down(2,:));
    hold off;
    ptss = ray2pix(fisheye_calib,rays_set{cube_idx}.rays);
    setPosition(cp3, [ptss(1,3) ptss(2,3)]);
end
end

