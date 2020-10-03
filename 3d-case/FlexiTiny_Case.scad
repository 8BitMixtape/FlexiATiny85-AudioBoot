



//14 9
WallHeight = 16;
BaseHeight = 1.2;

   
 
  linear_extrude(height = WallHeight, center = false, convexity = 10, twist = 0) import("TinyCase_wall.dxf", convexity=3);
  
  translate([0,0,0]) linear_extrude(height = BaseHeight, center = false, convexity = 10, twist = 0) import("TinyCase_base.dxf", convexity=3);
 


