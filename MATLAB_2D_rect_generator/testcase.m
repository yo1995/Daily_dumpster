clear;clc;
image_x_width_cm = 20;
nx = 128;
ny = 256;
rect_x_width_cm = 2;
rect_y_width_cm = 5;
rect_ii_center_cm = 60;
rect_jj_center_cm = 20;

odd_indicator = false;  % odd-width - true, even-width - false
result = my_rect(image_x_width_cm, nx, ny, rect_ii_center_cm, rect_jj_center_cm, rect_x_width_cm, rect_y_width_cm, odd_indicator);

rect_real = zeros(nx, ny);
imagesc(real(result))
axis equal tight
grid on