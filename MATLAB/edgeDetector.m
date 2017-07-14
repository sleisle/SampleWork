% Sean Leisle
% edgeDetector implements a simple edge detector algorithm for 2D signals

function [ y ] = edgeDetector( x )
    [a, b, c] = size(x);
    y = zeros(a+4,b+4,c);

    h1 = (1/20) .* [-1 0 1; -2 0 2; -1 0 1];
    h2 = (1/20) .* [1 2 1; 0 0 0; -1 -2 -1];
    
    for i = 1:c
        y(:,:,i) = conv2(conv2(x(:,:,i),h1),h2);
    end
end

