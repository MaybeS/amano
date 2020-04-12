function copy_object(object_idx)
global copy_obj;
global rays_set;

copy_obj = rays_set{object_idx};

 copy_obj.rays(:,1:4) = copy_obj.rays(:,1:4)*3;
 
 copy_obj.rays(1:2,1:4) = copy_obj.rays(1:2, 1:4) ... 
     -(copy_obj.rays(1:2, 3) - copy_obj.rays(1:2, 2))/4  ...
     -(copy_obj.rays(1:2, 1) - copy_obj.rays(1:2, 2))/4 ;
 
 copy_obj.rays = repmat(copy_obj.rays(:, 1:4), [1 2]);
 copy_obj.rays(3,5:end) = 3-copy_obj.vh;
 copy_obj.rays = copy_obj.rays ./ copy_obj.rays(3,:);
    
end