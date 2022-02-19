function [x,y]= circle(xc,yc,r)

theta= linspace(0,2*pi,30);

x= xc + r*cos(theta);
y= yc + r*sin(theta);