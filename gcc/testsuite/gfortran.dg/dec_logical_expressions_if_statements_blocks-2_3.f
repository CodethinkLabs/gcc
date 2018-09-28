! { dg-do compile }
! { dg-options "-std=legacy -fdec-non-logical-if" }

       function othersub1()
        integer*4 othersub1
        othersub1 = 1
       end
       function othersub2()
        integer*4 othersub2
        othersub2 = 2
       end
       program MAIN
        integer*4 othersub1
        integer*4 othersub2
c the if (integer) works here 
        if (othersub2()) then		! { dg-warning "" }
         write (*,*), 'othersub2 is true'
c but fails in the "else if"
        else if (othersub1()) then	! { dg-warning "" }
         write (*,*), 'othersub2 is false, othersub1 is true'
        endif
       end

