       ! { dg-do run }
       ! { dg-options "-std=legacy -fdec-hollerith-conversion" }

      program convert
      if (4HJMAC.ne.4HJMAC) stop 1 ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      if (4HJMAC.ne."JMAC") stop 2 ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      if (4HJMAC.eq."JMAN") stop 3 ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      if ("JMAC".eq.4HJMAN) stop 4 !  { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      if ("AAAA".eq.5HAAAAA) stop 5 ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      if ("BBBBB".eq.5HBBBB ) stop 6 ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      end program

