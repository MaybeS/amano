function delete_all()

global patch_set_u;
global patch_set_d;
global cp1;
global cp2;
global cp3;
global rays_set;

delete(cp1);
delete(cp2);
delete(cp3);

u_sz = size(patch_set_u);
for u_idx = u_sz(2) : -1 :1
    delete(patch_set_u{u_idx}.up);
    delete(patch_set_u{u_idx}.down);
    delete(patch_set_u{u_idx}.left);
    delete(patch_set_u{u_idx}.right);
    delete(patch_set_u{u_idx}.down);
    delete(patch_set_u{u_idx}.front);
    delete(patch_set_u{u_idx}.behind);
    patch_set_u(:,u_idx) = [];
end

d_sz = size(patch_set_d);
for d_idx = d_sz(2) : -1 :1
    delete(patch_set_d{d_idx}.up);
    delete(patch_set_d{d_idx}.down);
    delete(patch_set_d{d_idx}.left);
    delete(patch_set_d{d_idx}.right);
    delete(patch_set_d{d_idx}.down);
    delete(patch_set_d{d_idx}.front);
    delete(patch_set_d{d_idx}.behind);
    patch_set_d(:,d_idx) = [];
end

r_sz = size(rays_set);
for r_idx = r_sz(2):-1:1
    rays_set(:,r_idx) = [];
end


end