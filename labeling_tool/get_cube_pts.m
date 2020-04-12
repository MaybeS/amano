function pts_cube = get_cube_pts(K,rays_cube ,type , w, h )
th = 50 * pi / 180;
switch type
    case 2; rays_cube = rodrigues([th, 0, 0]) \ rays_cube;
    case 4; rays_cube = rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]) \ rays_cube;
    case 3; rays_cube = rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2]) \ rays_cube;
    case 1; rays_cube = (rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2])) \ rays_cube;
end

rays_cube = rays_cube./rays_cube(3,:);
pts_cube = K*rays_cube;

switch type
    case 2; pts_cube(1,:) = pts_cube(1,:) + w;
    case 4; pts_cube(1,:) = pts_cube(1,:) + w; pts_cube(2,:) = pts_cube(2,:) + h;
    case 3; pts_cube(2,:) = pts_cube(2,:) + h;
end

end