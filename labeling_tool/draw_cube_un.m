function draw_cube_un(pts_cube ,cube_func)
global patch_set_u;
%up, down

uset = {};
uset.up = fill(pts_cube(1,5:end), pts_cube(2,5:end), ...
                                    'green', 'FaceAlpha', 0.1,'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
uset.down = fill(pts_cube(1,1:4), pts_cube(2,1:4), ....
                                    'green', 'FaceAlpha', 0.1 , 'EdgeColor' , [1 1 1] , 'EdgeAlpha' , 0.3);
%front, behind
uset.front = fill([pts_cube(1, 1:2) pts_cube(1, 6:-1:5)], [pts_cube(2, 1:2) pts_cube(2, 6:-1:5)] , ... 
                                    'red' ,'FaceAlpha', 0.5 , 'EdgeColor' , [1 1 1],'EdgeAlpha' , 0.3);
uset.behind =fill([pts_cube(1, 3:4) pts_cube(1, 8:-1:7)], [pts_cube(2, 3:4) pts_cube(2, 8:-1:7)] , ...
                                    'blue' ,'FaceAlpha', 0.5, 'EdgeColor' , [1 1 1],'EdgeAlpha' , 0.3);
%left ,right
uset.left = fill([pts_cube(1, 1:3:4) pts_cube(1, 8:-3:5)], [pts_cube(2, 1:3:4) pts_cube(2, 8:-3:5)] , ...
                                    'green' ,'FaceAlpha', 0.1, 'EdgeColor' , [1 1 1],'EdgeAlpha' , 0.3);
uset.right =fill([pts_cube(1, 2:3) pts_cube(1, 7:-1:6)], [pts_cube(2, 2:3) pts_cube(2, 7:-1:6)] , ...
                                    'green' ,'FaceAlpha', 0.1, 'EdgeColor' , [1 1 1],'EdgeAlpha' , 0.3);

patch_set_u{end+1} = uset;

set(patch_set_u{end}.up,'ButtonDownFcn', cube_func);
set(patch_set_u{end}.down,'ButtonDownFcn', cube_func);
set(patch_set_u{end}.left,'ButtonDownFcn', cube_func);
set(patch_set_u{end}.right,'ButtonDownFcn', cube_func);
set(patch_set_u{end}.front,'ButtonDownFcn', cube_func);
set(patch_set_u{end}.behind,'ButtonDownFcn', cube_func);

end

