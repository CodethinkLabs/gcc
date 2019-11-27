! { dg-do run }
! { dg-options "-Wcharacter-truncation" }
! Tests the fix for PR31266: references to CHARACTER
! components lead to the wrong length being assigned to substring
! expressions.
type data
   character(len=5) :: c
end type data
type(data), dimension(5), target :: y
character(len=2), dimension(5) :: p
character(len=3), dimension(5) :: q

y(:)%c = "abcdef" ! { dg-warning "CHARACTER\\(6\\) truncated to CHARACTER\\(5\\)" }
p(1) = y(1)%c(3:) ! { dg-warning "CHARACTER\\(3\\) truncated to CHARACTER\\(2\\)" }
if (p(1).ne."cd") STOP 1

p(1) = y(1)%c  ! { dg-warning "CHARACTER\\(5\\) truncated to CHARACTER\\(2\\)" }
if (p(1).ne."ab") STOP 2

q = "xyz"
p = q ! { dg-warning "CHARACTER\\(3\\) truncated to CHARACTER\\(2\\)" }
if (any (p.ne.q(:)(1:2))) STOP 3
end
