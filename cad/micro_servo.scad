include <micro_servo_h.scad>
$fn=30;


// these are bracket constants
micro_servo();

module micro_servo() {
    // body
    cube([body_x,body_y,body_z]);
    // tabs
    difference() {
        color(body_color) translate([-tab_len,tab_offset,0]) cube([body_x + tab_len*2, tab_t, body_z]);
        translate( [-tab_len/2,tab_offset+tab_t+1, body_z/2]) rotate([90,0,0]) cylinder(r=tab_mount_r,h=tab_t+2);
        translate( [body_x+tab_len/2,tab_offset+tab_t+1, body_z/2]) rotate([90,0,0]) cylinder(r=tab_mount_r,h=tab_t+2);

    }

    // barrel
    color(body_color) translate( [shaft_offset, 0, body_z/2]) rotate([90,0,0]) cylinder( r=barrel_r, h=barrel_len);
    // shaft
    color(shaft_color) translate( [shaft_offset, -barrel_len, body_z/2]) rotate([90,0,0]) cylinder( r=shaft_r, h=shaft_l);
}