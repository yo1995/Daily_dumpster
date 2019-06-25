function result = my_rect(image_x_width_cm, nx, ny, rect_ii_center_cm, rect_jj_center_cm, ...
    rect_x_width_cm, rect_y_width_cm, is_odd)

% init
result = zeros(nx, ny);
image_x_cm = image_x_width_cm;
image_y_cm = image_x_width_cm / nx * ny;

% handle edge cases
if image_x_width_cm <= 0 || nx <= 0 || ny <= 0
    disp('invalid input for image dimensions.')
    return
end

if rect_jj_center_cm > ny || rect_ii_center_cm > nx || rect_jj_center_cm <= 0 || rect_ii_center_cm <= 0
    disp('invalid input for rect origin coord.')
    return
end

if rect_x_width_cm > image_x_cm || rect_y_width_cm > image_y_cm
    disp('invalid input for rect dimensions.')
    return
end

% rect pixel width and height
rect_width = floor(rect_x_width_cm / image_x_cm * nx);
rect_height = floor(rect_y_width_cm / image_y_cm * ny);
% rect origin pixel coord
rect_origin_x = rect_ii_center_cm;
rect_origin_y = rect_jj_center_cm;

% calculate bounds
rect_left = rect_origin_x - rect_width / 2;
rect_bottom = rect_origin_y;

% deal with oddity
if mod(rect_height, 2) == 1 && is_odd == false
    rect_height = rect_height - 1;
end
if mod(rect_height, 2) == 0 && is_odd == true
    rect_height = rect_height - 1;
end
if mod(rect_width, 2) == 1 && is_odd == false
    rect_width = rect_width - 1;
end
if mod(rect_width, 2) == 0 && is_odd == true
    rect_width = rect_width - 1;
end

if is_odd
    odd_string = 'odd-width';
else
    odd_string = 'even-width';
end

% note the matlab indices system is different! col, row instead of row, col
result(1:rect_height, 1:rect_width) = 1;
result = circshift(result, [rect_bottom, rect_left]);

% debug output
disp(['DEBUG: for ', odd_string, ' height = ', num2str(rect_height), ', width = ', num2str(rect_width)])


end