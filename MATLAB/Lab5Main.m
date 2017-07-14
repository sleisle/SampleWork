% Sean Leisle
% This lab uses photos to test image processing algorithms
% This tests an edge detector and a simple smoothing algorithm

clear all; close all; clc;

original = imread('photo2.jpg');
[x,y,z] = size(original);
% figure;
% imshow(uint8(original));
% title('Original Image');
grey = rgb2gray(original);

edges = edgeDetector(original);
% figure(2);
% imshow(edges);
% title('Edges of Image');
% 
smoothedEdges = smoother(edges);
% figure(3);
% imshow(smoothedEdges);
% title('smoothedEdges of Image');
% 
% greyEdges = edgeDetector(grey);
% smoothedGreyEdge = smoother(greyEdges);
% figure(4);
% imshow(smoothedGreyEdge);
% title('smoothedGreyEdge of Image');

% edgeMult = zeros(x,y,z);
% edgeMult(:,:,1) = (original(:,:,1) + im2uint8(100*smoothedGreyEdge(5:3460,5:4612)))/2;
% edgeMult(:,:,2) = (original(:,:,2) + im2uint8(100*smoothedGreyEdge(5:3460,5:4612)))/2;
% edgeMult(:,:,3) = (original(:,:,3) + im2uint8(100*smoothedGreyEdge(5:3460,5:4612)))/2;
% figure(5);
% imshow(edgeMult);
% title('Edge Multiply');
% 
% figure(6);
% imshow(uint8(origEmbossed));


for i = 1:x
    for j = 1:y
        if ((original(i,j,3) > 180) || ((original(i,j,1) > 160) && (original(i,j,2) > 160) && (original(i,j,3) > 160)))
            if (1)
                temp1 = original(i,j,1);
                temp2 = original(i,j,2);
                temp3 = original(i,j,3);
                original(i,j,1) = temp3;
                original(i,j,2) = temp1;
                original(i,j,3) = temp3;
            end
        end
        if (smoothedEdges(i+2,j+2,2) > 0.105)
            for a = 0:1
                for b = 0:1
                    original(i+a,j+b,:) = 0;
                end
            end
        end
    end
end
figure;
saveIm = uint8(smoother(original));

fourier = fft(saveIm);
[fx, fy, fz] = size(fourier);
maxF = max(max(fourier));

for i = 1:fx
    for j = 1:fy
        if (abs(abs(fourier(i,j,1)) + abs(fourier(i,j,2))+ abs(fourier(i,j,3))) > 1000000)
            fourier(i,j,:) = maxF(1,1,:) - fourier(i,j,:);
        end
    end
end
figure;
final = uint8(ifft((fourier)));
imwrite(final, 'lab5FinalImage.jpg');