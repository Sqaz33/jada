with ada.text_io;

procedure main is 

   procedure X(l: Integer)  is
      v: Integer;
   begin
      v := 1;
      ada.text_io.put_line(Integer'Image(1));
   end X;

   --  procedure X(l: Integer)  is
   --     v: Integer;
   --  begin
   --     v := 1;
   --     ada.text_io.put_line(Integer'Image(1));
   --  end X;


   --  function X(l: Integer) return Integer is
   --  begin
   --     ada.text_io.put_line(Integer'Image(2));
   --     return l;
   --  end X;


   function X(l: Integer) return Integer is
   begin
      ada.text_io.put_line(Integer'Image(2));
      return l;
   end X;


   function X(l: Integer; l2: Integer) return Integer is
   begin
      ada.text_io.put_line(Integer'Image(2));
      return l;
   end X;



   l: Integer;
begin
   l :=  X(1);
   X(1);  -- 2, 1
end main; 