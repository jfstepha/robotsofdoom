$fn=30;
handle_r = 8;
handle_l = 90;
head_r = 15;
head_l = 30;
pin_r = 4;
pin_rtol = 0.3;   // gap between pin and the hole
pin_ltol = 0.3;  // gap betwen the pin ends and the hinge
pin_head_r = pin_r * 2;
pin_head_l = 4;
hinge_t = 4;
hinge_tol = 0.5; // gap between the hinge and the handle
hinge_pin_y = pin_r*3;
servo_arm_l = 13;
drive_hole_r = 1;
barb_l = 4;
barb_r = 0.1;
slot_t = 1.5;
slot_l = barb_l + 4;



explode = 1;  //  set to 1 to print, 0 to see assembly

diff_tol = 1; // tolerance for diff operations

pin_l = handle_r*2 + hinge_tol*2 + hinge_t * 2 + pin_ltol*2;  // just the shaft part
pin_offset = pin_r * 2;
handle_z = head_l-handle_r;
pin_y = -handle_l + pin_offset;

servo_l = 24;
servo_w = 13;
servo_z = handle_z + handle_r + 5;
servo_y = hinge_pin_y + handle_r;
servo_oh = 8;

/***** hammer *****/
// head
cylinder( r = head_r, h = head_l);
// handle
difference() {
    // handle itself
    translate( [0, 0, handle_z ] )                             rotate( [90,0,0])  cylinder( r = handle_r, h = handle_l);
    // hold for pin
    translate( [-pin_l/2, pin_y, handle_z] )                   rotate( [0,90,0] ) cylinder( r=pin_r+pin_rtol, h=pin_l);
    // drive hole
    translate( [-pin_l/2, pin_y + servo_arm_l * 2, handle_z] ) rotate( [0,90,0] ) cylinder( r=drive_hole_r, h=pin_l);
    
}

/**** hinge ****/
hinge_w = handle_r*2 + hinge_tol * 2;  // inner
hinge_l = servo_y+servo_w+servo_oh;
hinge_y = pin_y - hinge_w /2 ;
color("blue") translate( [explode*head_r*3 - hinge_l/2, (1-explode) * hinge_y,0 ] ) {
    
    // base
                                                                             cube([hinge_w,hinge_l,hinge_t]);
    difference() {
        union() {
            translate( [hinge_w, 0,0])                                       cube( [hinge_t, hinge_l, handle_z] );
            translate( [-hinge_t, 0,0])                                      cube( [hinge_t, hinge_l, handle_z] );
            // sides
//            translate( [hinge_w, hinge_w/2, handle_z] )  rotate( [0,90,0])   cylinder( r=hinge_w/2, h=hinge_t);
//            translate( [-hinge_t, hinge_w/2, handle_z] ) rotate( [0,90,0])   cylinder( r=hinge_w/2, h=hinge_t);
            translate( [hinge_w, 0, handle_z] )                      cube( [hinge_t, hinge_l, servo_z - handle_z]);
            translate( [-hinge_t, 0, handle_z] )                     cube( [hinge_t, hinge_l, servo_z - handle_z]);
        }
        // hole for pin
        translate( [-hinge_t, hinge_pin_y, handle_z] )     rotate( [0,90,0])   cylinder( r=pin_r + pin_rtol, h=pin_l);
        
    }
    // servo_holder
    translate( [hinge_w,0,servo_z])                                             cube( [hinge_t, servo_y, servo_l] ) ;
    translate( [hinge_w,servo_y+servo_w,servo_z])                               cube( [hinge_t, servo_oh, servo_l] ) ;
    translate( [-hinge_t,0,servo_z])                                            cube( [hinge_t, servo_y, servo_l] ) ;
    translate( [-hinge_t,servo_y+servo_w,servo_z])                              cube( [hinge_t, servo_oh, servo_l] ) ;
    translate( [-hinge_t,0, servo_z + servo_l])                                 cube( [hinge_t, servo_y + servo_w + servo_oh, servo_oh] ) ;
    translate( [ hinge_w,0, servo_z + servo_l])                                 cube( [hinge_t, servo_y + servo_w + servo_oh, servo_oh] ) ;
   
}


/**** pin ****/
translate( [ -explode*head_r*2 + (1-explode) * -(pin_l/2 + pin_head_l ), (1-explode) * hinge_y + hinge_w/2, (1-explode)*handle_z] ) rotate( [0, 90*(1-explode), 0] )  {
    // pin head
    cylinder( r=pin_head_r, h=pin_head_l) sphere( r=0.5);
    difference() {
        union() {
            // pin shaft
            translate( [ 0,0, pin_head_l] ) cylinder( r=pin_r, h=pin_l) sphere( 0.5);
            // pin barb
            translate( [0,0,pin_head_l + pin_l] ) cylinder( r1=pin_r+barb_r+pin_rtol, r2=pin_r,h=barb_l );
        }
        // slot
        translate( [-pin_r-barb_r-pin_rtol,-slot_t/2, pin_l+pin_head_l+barb_l-slot_l]) color("red") cube( [pin_r*2+barb_r*2+pin_rtol*2, slot_t, slot_l] ) ;
    }
    
}


