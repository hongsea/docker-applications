<?php
/** @package Showcase::Model::DAO */

/** import supporting libraries */
require_once("verysimple/Phreeze/Phreezable.php");
require_once("AlarmMap.php");

/**
 * AlarmDAO provides object-oriented access to the alarm_report table.  This
 * class is automatically generated by ClassBuilder.
 *
 * WARNING: THIS IS AN AUTO-GENERATED FILE
 *
 * This file should generally not be edited by hand except in special circumstances.
 * Add any custom business logic to the Model class which is extended from this DAO class.
 * Leaving this file alone will allow easy re-generation of all DAOs in the event of schema changes
 *
 * @package Showcase::Model::DAO
 * @author ClassBuilder
 * @version 1.0
 */
class AlarmDAO extends Phreezable
{
	/** @var int */
	public $Id;

	/** @var string */
	public $Node;

	/** @var string */
	public $Sensor;

	/** @var string */
	public $Sensoreenheid;
	
	/** @var string */
	public $Alarmtrigger;

	/** @var string */
	public $Observatiewaarde;

	/** @var date */
	public $Observatietijdstip;



}
?>