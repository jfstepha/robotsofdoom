include<micro_servo_h.scad>
use<micro_servo.scad>;

$fn=30;
t=3;
base_tab_w =10;
mnt_hole_r = 2;
// color("red") micro_servo();

difference () {
    translate([ -tab_len, tab_t+tab_offset, -t ] ) cube([body_x + 2 * tab_len, body_y/2-tab_t, body_z+t]);
    cube([body_x, body_y, body_z]);
    translate( [-tab_len/2,tab_offset+tab_t+body_y, body_z/2]) rotate([90,0,0]) cylinder(r=tab_mount_r,h=body_y);
    translate( [body_x+tab_len/2,tab_offset+tab_t+body_y, body_z/2]) rotate([90,0,0]) cylinder(r=tab_mount_r,h=tab_t+body_y);
}

difference() {
    translate([ -tab_len-base_tab_w, 0, -t] ) cube( [body_x + 2 * tab_len+2*base_tab_w, body_y, t] ) ;
    translate([ -tab_len-base_tab_w/2,body_y/2,-t-1 ] ) cylinder( r=mnt_hole_r, h=t+2);
    translate([ body_x+base_tab_w/2+tab_len,body_y/2,-t-1 ] ) cylinder( r=mnt_hole_r, h=t+2);

}