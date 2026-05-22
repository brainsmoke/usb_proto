
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
					$_graft = "base_add";
					children();
				}
				union()
				{
					$_graft = "base_remove";
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
			$_graft = "remove";
			children();
		}
	}

	if ($preview)
	{
		$_graft = "preview";
		children();
	}
}

module graft_base_add()
{
	if ($_graft == "base_add")
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
	if ($_graft == "remove")
	{
		$_graft = undef;
		children();
	}
}

module graft_base_remove()
{
	if ($_graft == "base_remove")
	union()
	{
		$_graft = undef;
		children();
	}
}

module graft_preview()
{
	if ($_graft == "preview")
	union()
	{
		$_graft = undef;
		children();
	}
}
