       ! { dg-options "-std=extra-legacy" }

      program convert
      REAL*4 a
      INTEGER*4 b
      b = 1000
      print *, 4HJMAC.eq.4HJMAC ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      print *, 4HJMAC.eq."JMAC" ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      print *, 4HJMAC.eq."JMAN" ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      print *, "JMAC".eq.4HJMAN !  { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      print *, "AAAA".eq.5HAAAAA ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      print *, "BBBBB".eq.5HBBBB ! { dg-warning "Promoting argument for comparison from HOLLERITH to CHARACTER at" }
      end program

