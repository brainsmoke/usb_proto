
module graft()
{
	if (!is_undef($_graft))
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
					$_graft = "base";
					children();
				}
				union()
				{
					$_graft = "base_carve";
					children();
				}
			}
			union()
			{
				$_graft = "add";
				children();
			}
		}
		union()
		{
			$_graft = "carve";
			children();
		}
	}
}

module graft_base()
{
	if ($_graft == "base")
	{
		$_graft = undef;
		children();
	}
}

module graft_add()
{
	if ($_graft == "add")
	{
		$_graft = undef;
		children();
	}
}

module graft_remove()
{
	if ($_graft == "carve")
	{
		$_graft = undef;
		children();
	}
}

module graft_base_carve()
{
	if ($_graft == "base_carve")
	union()
	{
		$_graft = undef;
		children();
	}
}
