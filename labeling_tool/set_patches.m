function set_patches(face_type ,select_idx)
global patch_set_u;
global patch_set_d;


front_alpha = 0.5;
side_alpha = 0.1;
back_alpha  = 0.5;

front_color = 'red';
side_color = 'green';
back_color = 'bule';


if face_type == 1
    front_color = 'red';
    left_color = 'green';
    right_color = 'green';
    back_color = 'blue';
    
elseif face_type == 2
  front_color = 'blue';
    left_color = 'green';
    right_color = 'green';
    back_color = 'red';
    
    
elseif face_type == 3 %left

     left_color = 'red';
    right_color = 'blue';
    back_color = 'green';
    front_color = 'green';
    
    front_alpha = 0.1;
    side_alpha = 0.5;
    back_alpha  = 0.1;
else  %right
       left_color = 'blue';
    right_color = 'red';
    back_color = 'green';
    front_color = 'green';
    
    front_alpha = 0.1;
    side_alpha = 0.5;
    back_alpha  = 0.1;
    
end

set(patch_set_u{select_idx}.front,'FaceColor',front_color , 'FaceAlpha', front_alpha);
set(patch_set_u{select_idx}.behind,'FaceColor',back_color , 'FaceAlpha', back_alpha );
set(patch_set_u{select_idx}.left,'FaceColor',left_color, 'FaceAlpha' , side_alpha);
set(patch_set_u{select_idx}.right,'FaceColor',right_color, 'FaceAlpha',side_alpha);

set(patch_set_d{select_idx}.front , 'FaceColor' , front_color , 'FaceAlpha', front_alpha);
set(patch_set_d{select_idx}.behind , 'FaceColor' , back_color , 'FaceAlpha', back_alpha);
set(patch_set_d{select_idx}.left , 'FaceColor' , left_color , 'FaceAlpha', side_alpha);
set(patch_set_d{select_idx}.right , 'FaceColor' , right_color, 'FaceAlpha' , side_alpha);

end
