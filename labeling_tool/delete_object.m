function delete_object(obj_idx)
global patch_set_u;
global patch_set_d;
global rays_set;
global cp1;
global cp2;
global cp3;

delete(cp1);
delete(cp2);
delete(cp3);

delete(patch_set_u{obj_idx}.up);
delete(patch_set_u{obj_idx}.down);
delete(patch_set_u{obj_idx}.left);
delete(patch_set_u{obj_idx}.right);
delete(patch_set_u{obj_idx}.down);
delete(patch_set_u{obj_idx}.front);
delete(patch_set_u{obj_idx}.behind);


delete(patch_set_d{obj_idx}.up);
delete(patch_set_d{obj_idx}.down);
delete(patch_set_d{obj_idx}.left);
delete(patch_set_d{obj_idx}.right);
delete(patch_set_d{obj_idx}.down);
delete(patch_set_d{obj_idx}.front);
delete(patch_set_d{obj_idx}.behind);

rays_set(:,obj_idx) = [];
patch_set_u(:,obj_idx) = [];
patch_set_d(:,obj_idx) = [];

end