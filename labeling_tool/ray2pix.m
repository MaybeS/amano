function p = ray2pix(calib, r)

fx = calib.f(1);  fy = calib.f(2);
cx = calib.c(1);  cy = calib.c(2);
k = calib.k;
is_fisheye = false;
if numel(k) < 5, k((end+1):5) = 0; end;
if isfield(calib, 'fisheye'), is_fisheye = calib.fisheye; end;

if (size(r,1) == 2), r(3,:) = 1; end;

if is_fisheye
    % Fisheye x-theta modelsu
    n = sqrt(sum(r.^2, 1));
    r = r ./ repmat(n + eps, [3,1]);  % Normalize.
    d = sqrt(sum(r(1:2,:).^2, 1));
    th = atan2(d, r(3,:));  % Compute theta from x.
    th = th .* (1 + th .* (k(1) + th .* k(2)));  % Apply radial ditortion.
    q = r(1:2,:) .* repmat(th ./ (d + eps), [2,1]);
else
    x = r(1,:) ./ r(3,:);
    y = r(2,:) ./ r(3,:);
    x2 = x.^2;
    y2 = y.^2;
    xy = x .* y;
    r2 = x2 + y2;
    
    rc = 1 + r2 .* (k(1) + r2 .* (k(2) + r2 * k(5)));
    tcx = k(3) * 2*xy + k(4) * (3*x2 + y2);
    tcy = k(3) * (x2 + 3*y2) + k(4) * 2*xy;
    
    q = [rc.*x + tcx; rc.*y + tcy];
end

K = [fx, 0, cx; 0, fy, cy; 0, 0, 1];
q(3,:) = 1;
p = K * q;
    
end
