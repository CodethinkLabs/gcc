       ! { dg-options "-std=extra-legacy" }

      program convert
      INTEGER*4 b
      b = 5HRIVET
      print *, 4HJMAC.eq.400 ! { dg-warning "Promoting argument for comparison from HOLLERITH" }
      print *, 4HRIVE.eq.1163282770 ! { dg-warning "Promoting argument for comparison from HOLLERITH" }
      print *, b
      print *, 1163282770.eq.4HRIVE ! { dg-warning "Promoting argument for comparison from HOLLERITH" }
      end program

