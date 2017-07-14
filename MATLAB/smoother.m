% Sean Leisle
% smoother implements a simple smoothing algorithm for 2D signals

function [ y ] = smoother( x )
    [a, b, c] = size(x);
    y = zeros(a+4,b+4,c);

    h1 = ((1/25) * ones(5));
    
    for i = 1:c
        y(:,:,i) = conv2(x(:,:,i),h1);
    end
end

