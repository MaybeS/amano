function v_idx = check_un_view(K,rays , w, h ,th)

v_idx = -1;
u = (rodrigues([th, 0, 0])) \rays;
d = (rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]))\ rays;
r = (rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2])) \ rays;
l = (rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2])) \ rays;

u = K*(u./u(3,:));
d = K*(d./d(3,:));
r = K*(r./r(3,:));
l = K*(l./l(3,:));

i_set = inpolygon([l(1,:)' u(1,:)' r(1,:)' d(1,:)'], [l(2,:)' u(2,:)' r(2,:)' d(2,:)'] ...
        , [1 w w 1] , [1 1 h h]);
    
[m, v_idx] = max(sum(i_set,1));

if m == 0 
    v_idx = -1;
end

end