
module sculpt()
{
	if (!is_undef($_sculpt))
		children();
	else
	difference()
	{
		union()
		{
			difference()
			{
				union()
				{
					$_sculpt = "base";
					children();
				}
				union()
				{
					$_sculpt = "base_carve";
					children();
				}
			}
			union()
			{
				$_sculpt = "add";
				children();
			}
		}
		union()
		{
			$_sculpt = "carve";
			children();
		}
	}
}

module sculpt_base()
{
	if ($_sculpt == "base")
	{
		$_sculpt = undef;
		children();
	}
}

module sculpt_add()
{
	if ($_sculpt == "add")
	{
		$_sculpt = undef;
		children();
	}
}

module sculpt_carve()
{
	if ($_sculpt == "carve")
	{
		$_sculpt = undef;
		children();
	}
}

module sculpt_base_carve()
{
	if ($_sculpt == "base_carve")
	union()
	{
		$_sculpt = undef;
		children();
	}
}
