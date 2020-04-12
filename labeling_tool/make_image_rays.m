% make_image_rays : type = u,d,r,l, U,D,R,L
function [rays, ff] = make_image_rays(w, h, type, varargin)

opt = struct(varargin{:});
if ~isfield(opt, 'view_th'), opt.view_th = 45; end;
if ~isfield(opt, 'y_offset'), opt.y_offset = 0; end;
if ~isfield(opt, 'y_coef'), opt.y_coef = 1.25; end;
if ~isfield(opt, 'z_dist'), opt.z_dist = 0.8; end;

[x, y] = meshgrid((1:w) - 1, (1:h) - 1);
xr = x(end:-1:1, :);
n = numel(x);

ff = opt.z_dist * h;

th = opt.view_th * pi / 180;
y_off = h * opt.y_offset;
y_coef = opt.y_coef;

switch type
    %-- Frontal view
    case 'R'; rays = [ff * ones(n,1), x(:) - w/2, y_coef * (y(:) + y_off)]';
    case 'L'; rays = [ff * -ones(n,1), -(xr(:) - w/2), y_coef * (y(:) + y_off)]';
    otherwise
        
%-- Normal views
K = [ff, 0, w / 2; 0, ff, h / 2; 0, 0, 1];
rays = K \ [x(:), y(:), ones(n,1)]';
switch type
    case 'u'; rays = rodrigues([th, 0, 0]) * rays;
    case 'd'; rays = rodrigues([-th, 0, 0]) * rodrigues([0, 0, pi]) * rays;
    case 'r'; rays = rodrigues([0, th, 0]) * rodrigues([0, 0, pi/2]) * rays;
    case 'l'; rays = rodrigues([0, -th, 0]) * rodrigues([0, 0, -pi/2]) * rays;
end
end

end
