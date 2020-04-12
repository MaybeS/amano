function save_csv(handles)
global rays_set;
global fisheye_calib;
global file_name;
% serialize ==> fn, id, face, height, x ,y (num :3 -> 6) ,side type
% total : 11

% make directory path
[filepath, name,ext] = fileparts(file_name);
save_dir ="";
if ismac || isunix
   s = split(filepath, "/");
   ss = size(s);
   save_dir = "dump/"+ s(ss(1)-1) + "/" + s(ss(1));
elseif ispc
   s = split(filepath, "\");
   ss = size(s);
   save_dir = "dump\"+ s(ss(1)-1) + "\" + s(ss(1));
end

%mkdir
if exist(save_dir , 'dir') ~=7
   mkdir(char(save_dir ));
end

save_path= "";

%save path
if ismac || isunix
    save_path = save_dir+"/"+name+".csv";
elseif ispc
    save_path = save_dir+"\"+name+".csv";
end

if exist(save_path , 'file')
    answer = questdlg('file 이 존재하는데 덮어쓸까요?','question dlg','yes' ,'no','cancel');
    if(strcmp(answer, 'no') || strcmp(answer, 'cancel'))
        return;
    end
end
axes(handles.axes1);
% xlabel('Saving ....', 'Color' ,'r');

rsz = size(rays_set);
dump_data = zeros(rsz(2), 11);
if rsz(2) <= 0
    %     xlabel('저장할 것이 없습니다 ..', 'Color' ,'r');
    dump_data = zeros(1, 11);
else
    
    for i = 1 : rsz(2)
        pts_set = ray2pix(fisheye_calib, rays_set{i}.rays);
        dump_data(i, :) = [0 i rays_set{i}.face rays_set{i}.vh  ...
            reshape(pts_set(1:2,1:3) , [1 6]) rays_set{i}.type_char];
    end
end
csvwrite(save_path, dump_data);
xlabel("Saved !");
end