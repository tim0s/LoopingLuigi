difference() {

cube([100, 80, 2]);

translate([15, 15, -0.5])
cube([26.7, 19.4, 3]);
    
translate([60, 25, -0.5])
cylinder(d=7, h=3, $fn=20);

}