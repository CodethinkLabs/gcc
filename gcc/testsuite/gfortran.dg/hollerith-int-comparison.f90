       ! { dg-options "-std=extra-legacy" }

      program convert
      INTEGER*4 b
      b = 5HRIVET ! { dg-warning "Legacy Extension: Hollerith constant|Conversion from HOLLERITH to INTEGER|too long to convert" }
      print *, 4HJMAC.eq.400 ! { dg-warning "Legacy Extension: Hollerith constant|Promoting argument for comparison from character|Conversion from HOLLERITH to INTEGER" }
      print *, 4HRIVE.eq.1163282770 ! { dg-warning "Legacy Extension: Hollerith constant|Promoting argument for comparison from character|Conversion from HOLLERITH to INTEGER" }
      print *, b
      print *, 1163282770.eq.4HRIVE ! { dg-warning "Legacy Extension: Hollerith constant|Promoting argument for comparison from character|Conversion from HOLLERITH to INTEGER" }
      end program

