<?php

	function unhtmlentities( $entities ) // {{{
	{
		static $trans;
		if(!$trans)
		{
			$trans = array_flip( get_html_translation_table( HTML_ENTITIES ) );
		}
		return strtr( $entities, $trans );
	} // }}}
	function strip_first_slash( $str ) // {{{
	{
		if( substr( $str, 0, 1 ) == '/' )
			return substr( $str, 1 );
		else
			return $str;
	} // }}}
	
	/**
	 * Remove slashes from the ends of a string
	 *
	 * @param string $str
	 * @return string $str_with_slashes_trimmed
	 */
	function trim_slashes( $str ) // {{{
	{
		if( substr( $str, 0, 1 ) == '/' )
			$str = substr( $str, 1 );
		if( substr( $str, -1 ) == '/' )
			$str = substr( $str, 0, -1 );
		return $str;
	} // }}}
	
	/**
	 * Strip slashes *if magic quotes are turned on*
	 *
	 * Most of the Reason and Carl Util tools expect to work with unescaped values
	 * and will double-escape values if they are written to the db that way.
	 * It is good practice to run userland values through this function so that code 
	 * will work whether or not magic quotes are on.
	 *
	 *
	 * @param string $string_that_may_be_escaped
	 * @return string $unescaped_string
	 */
	function conditional_stripslashes($value)
	{
		if (!get_magic_quotes_gpc())
		{
			return $value;
		}
		else 
		{
			if (is_array($value))
			{
				foreach($value as $k => $v)
				{
					$z = ((is_array($v)) ? array_map("conditional_stripslashes",$v) : conditional_stripslashes($v));
					$value[$k] = $z;
				}
				return $value;
			}
			else
			{
				return stripslashes($value);
			}
		}
	}

	/**
	 * Escape/encode reserved LDAP filter characters for inclusion in an LDAP filter
	 *
	 * replaces '\\','@','*','(', and ')' with, respectively, '\5c','\40',	'\2a', '\28', and '\29'.
	 *
	 * @param string $string_to_be_escaped
	 * @return string $escaped_string
	 */
	function ldap_escape($value)
	{
		return str_replace(array('\\','@','*','(',')'), array('\5c','\40',	'\2a', '\28','\29'), $value);
	}
	
	/**
	 * Replace escaped/encoded LDAP filter characters with the real ASCII characters they represent
	 *
	 * replaces '\5c', '\40', '\2a', '\28', and '\29' with, respectively, '\\', '@', '*', '(', and ')'.
	 *
	 * @param string $escaped_string
	 * @return string $unescaped_string
	 */
	function ldap_unescape($value)
	{
		return str_replace(array('\5c','\40',	'\2a', '\28','\29'),array('\\','@','*','(',')'), $value);
	}
	
	/**
	 * Moved from reason function_libraries/util.php
	 */
	function carl_clean_vars( &$vars, $rules ) // {{{
	// Returns an array which takes the values of the keys in Vars of
	// the keys set in Settings, and runs the cleaning function
	// specified in the value of settings
	{
		$return = array();
		foreach ( $rules as $key => $func_and_args )
		{
			// very important that this is isset() and not empty().  if we use !empty(), the request that is sent to the
			// module is not quite right.  _REQUEST can have keys that have a set (perhaps empty) value.  This is
			// crucial for systems like Disco and Plasmature that need to know if a variable was set to nothing in the
			// form.  Otherwise, default values can come cropping back up unexpectedly. --dh
			if ( isset($vars[$key]) )
			{
				$func = $func_and_args['function'];
				if ( !empty($func_and_args['extra_args']) )
				{
					$extra_args = $func_and_args['extra_args'];
					$return[$key] = $func( $vars[$key], $extra_args );
				}
				else
					$return[$key] = $func( $vars[$key] );
			}
		}
		return $return;
	} // }}}

	// 
	// The following functions should all conform to the following interface:
	//   [turn_into_|check_against_]xxxxxxx($val, $extra_[arg|args])
	//
	// If there's only one extra_arg, then it can expect whatever type is appropriate.
	// If there's more than one extra_arg, then $extra_args should be an associative array.
	//
	function check_against_array($val, $array) //{{{
	{
		if ( !empty($val) && !in_array($val, $array) )
			return NULL;
		else
			return $val;
	} // }}}

	function check_against_regexp($val, $array)
	{
		$common_regexp['alphanumeric'] = '/^[a-z0-9]*$/i';
		$common_regexp['naturalnumber'] = '/^[0-9]*$/i';
		$common_regexp['safechars'] = '/^[a-z0-9_-]*$/i';
		$common_regexp['email'] = '/^[0-9A-Za-z_\-]+[@][0-9A-Za-z_\-]+([.][0-9A-Za-z]+)([.][A-Za-z]{2,3}){0,1}$/x';
		foreach ($array as $this_regexp)
		{
			$this_regexp = isset($common_regexp[$this_regexp]) ? $common_regexp[$this_regexp] : $this_regexp;
			if (!preg_match( $this_regexp, $val)) return NULL;
		}
		return $val;
	}

	function turn_into_array($val, $dummy = NULL) //{{{
	{
		if ( is_array($val) )
			return $val;
		else
			return array();
	} // }}}
	function turn_into_int($val, $extra_args = NULL) //{{{
	{
		settype($val, 'integer');
		if (is_array($extra_args))
		{
			if (isset($extra_args['zero_to_null']) && ($extra_args['zero_to_null'] == 'true') && ($val == 0)) return NULL;
			if (isset($extra_args['default']) && ($val == 0)) return $extra_args['default'];
		}
		return $val;
	} // }}}
	
	function turn_into_string($val, $dummy = NULL) //{{{
	{
		settype($val, 'string');
		return $val;
	} // }}}
	function turn_into_date($val, $format = 'Y-m-d') //{{{
	{
		return prettify_mysql_datetime($val, $format);
	} // }}}
	function turn_into_yyyy_mm($val, $format = 'Y-m') //{{{
	{
		return prettify_mysql_datetime($val.'-01', $format);
	} // }}}

	// val = value to sanitize
	// array (array('pattern' => $regexp, 'replace' => '_'))
	function sanitize_by_replace_regexp($val, $array)
	{
		$common_regexp['filename'] = '/[^a-z0-9._]/i';
		if (!is_array(current($array))) $rules_array[] = $array;
		else ($rules_array = $array);
		foreach ($rules_array as $this_regexp)
		{
			$this_regexp['pattern'] = isset($common_regexp[$this_regexp['pattern']]) 
											   ? $common_regexp[$this_regexp['pattern']]
											   : $this_regexp['pattern'];
			$val = preg_replace( $this_regexp['pattern'], $this_regexp['replace'], $val);
		}
		return $val;
	}
	/**
	 * Cleans up a filename so that it can be hosted on the web without worry about encoding wierdnesses
	 * Really just an easy-to-use wrapper for the more powerful sanitize_by_replace_regexp().
	 * @param string $filename the filename string needing sanitization
	 * @return string $clean_filename the filename with iffy chars replaced with underscores
	 */
	function sanitize_filename_for_web_hosting($filename)
	{
		$rules = array('pattern' => 'filename', 'replace' => '_');
		return sanitize_by_replace_regexp($filename, $rules);
	}
	
	/**
	 * Sanitizes HTML using the function specified in the package_settings.php HTML_SANITIZATION_FUNTION
	 * - defaults to using get_safer_html_html_purifier
	 * @param string html string needing sanitization
	 * @return string sanitized html string
	 */
	function carl_get_safer_html($string)
	{
		if (defined('HTML_SANITIZATION_FUNCTION'))
		{
			$func_name = HTML_SANITIZATION_FUNCTION;
		}
		else
		{
			trigger_error('The HTML_SANITIZATION_FUNCTION constant in package_settings.php is not defined - defaulting to get_safer_html_html_purifier', WARNING);
			$func_name = 'get_safer_html_html_purifier';
		}
		return $func_name($string);
	}

	function get_safer_html_html_purifier($string)
	{
		require_once( HTML_PURIFIER_INC . 'htmlpurifier.php' );
		$config = HTMLPurifier_Config::createDefault();
		$config->set('HTML', 'Doctype', 'HTML 4.01 Strict');
		$config->set('HTML', 'TidyLevel', 'heavy');
		
		// lets transform b to strong and i to em
		$def =& $config->getDefinition('HTML');
		$def->info_tag_transform['b'] = new HTMLPurifier_TagTransform_Simple('strong');
		$def->info_tag_transform['i'] = new HTMLPurifier_TagTransform_Simple('em');

		$purifier = new HTMLPurifier($config);

    	return $purifier->purify( $string );
	}

	function get_safer_html($string)
	{
		return carl_get_safer_html($string);
	}

	function get_safer_html_html_safe($string)
	{
		require_once('HTML/Safe.php');
		$parser = new HTML_Safe();
		$parser->attributes = array('dynsrc');
		return $parser->parse($string);
	}

?>
