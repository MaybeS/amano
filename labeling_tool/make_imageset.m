function [img_out] = make_imageset(img_in)
% image_file = '../data/amano_fisheye/CAM01/CAM01_01.jpg';
% image_file = '../data/amano_fisheye/CAM02/CAM02_01.jpg';
% image_file = '../data/amano_fisheye/slots_46.jpg';
% image_file = 'sample2.jpg';

I = mean(single(img_in), 3) / 255;
sz = fliplr(size(I));

%%


% f = 0.4 * sz(1);
% c = sz / 2 + [25,-30];
% k = -0.25;

f = 0.26 * sz(1);  % Fotec lens
c = sz / 2 + [77, 33];
k = [0.07111501 -0.07150972 0 0 0];

fisheye_calib = struct('f', [f, f], 'c', c, 'k', k, 'size', sz, 'fisheye', true);

% deg2rad = pi / 180;
% [thx, thy] = meshgrid((0:10:359) * deg2rad, (0:10:120) * deg2rad);

% fisheye_rays = [cos(thx(:)) .* sin(thy(:)), sin(thx(:)) .* sin(thy(:)), cos(thy(:))]';
% fisheye_rays = cat(2, [0, 0, 1]', fisheye_rays);
% if exist('rot', 'var')
%     rays = rodrigues(rot)' * rays;
%     rays = rays ./ repmat(rays(3,:), [3,1]);
% end

% fisheye_pts = ray2pix(fisheye_calib, fisheye_rays);

% fisheye_pts_x = reshape(fisheye_pts(1,:), size(thx));
% fisheye_pts_y = reshape(fisheye_pts(2,:), size(thx));
%  num_row = size(thx, 1);
% cmap = jet(num_row);

% figure(1);
% imshow(I);
% hold on;
% plot(fisheye_pts(1,:), fisheye_pts(2,:), 'rx');

%draw axis line
% plot([0,sz(1)], [c(2),c(2)], 'w-', [c(1),c(1)], [0,sz(2)], 'w-');

% for i = 1:num_row
%     %     plot(fisheye_pts_x(i,:), fisheye_pts_y(i,:), 'x-', 'Color', cmap(i,:), 'LineWidth', 1);
% end
% plot(fisheye_pts_x, fisheye_pts_y, 'k-', 'LineWidth', 1);
% hold off;

%%

w = 960;
h = 480;
types = {'lu', 'rd'};  % u,d,r,l, U,D,R,L

num_row = numel(types);
num_col = numel(types{1});
img_out = zeros(h * num_row, w * num_col);

for r = 1:num_row
    for c = 1:num_col
        [rays, ff] = make_image_rays(w, h, types{r}(c), 'view_th', 50);
        if exist('rot', 'var')
            rays = rodrigues(rot)' * rays;
            rays = rays ./ repmat(rays(3,:), [3,1]);
        end
        pts = ray2pix(fisheye_calib, rays);
        img_out((r - 1) * h + (1:h), (c - 1) * w + (1:w)) = ...
            reshape(interp2(I, pts(1,:), pts(2,:), 'linear'), [h, w]);
    end
end



end