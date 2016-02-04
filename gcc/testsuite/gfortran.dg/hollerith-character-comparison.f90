       ! { dg-options "-std=extra-legacy" }

      program convert
      REAL*4 a
      INTEGER*4 b
      b = 1000
      print *, 4HJMAC.eq.4HJMAC ! { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH" }
      print *, 4HJMAC.eq."JMAC" ! { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH" }
      print *, 4HJMAC.eq."JMAN" ! { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH" }
      print *, "JMAC".eq.4HJMAN !  { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH" }
      print *, "AAAA".eq.5HAAAAA ! { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH" }
      print *, "BBBBB".eq.5HBBBB ! { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH" }

      end program

