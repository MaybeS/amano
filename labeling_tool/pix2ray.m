function np = pix2ray(calib, p)

fx = calib.f(1);  fy = calib.f(2);
cx = calib.c(1);  cy = calib.c(2);
k = calib.k;
is_fisheye = false;
if numel(k) < 5, k((end+1):5) = 0; end;
if isfield(calib, 'fisheye'), is_fisheye = calib.fisheye; end;

K = [fx, 0, cx; 0, fy, cy; 0, 0, 1];
p(3,:) = 1;
p = K \ p;
np = p;

if is_fisheye
    % Fisheye x-theta model
    for  i = 1 : 1000
        np_tmp = np;
        n = sqrt(sum(np.^2, 1));
        np = np ./ repmat(n + eps, [3,1]);  % Normalize.
        d = sqrt(sum(np(1:2,:).^2, 1));
        th = atan2(d, np(3,:));  % Compute theta from x.
        th = th .* (1 + th .* (k(1) + th .* k(2)));  % Apply radial ditortion.
        np =np(1:2,:) .* repmat(th ./ (d + eps), [2,1]);
        
        np(3,:) = 1;
        err = np - p;
        np = np_tmp - err;
        
        if norm(err)< 1e-10 
            break; 
        end
        
    end
end

end