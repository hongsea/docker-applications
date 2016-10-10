<?php
/** @package Showcase::Model::DAO */

/** import supporting libraries */
require_once("verysimple/Phreeze/Phreezable.php");
require_once("SensorMap.php");

/**
 * SensorDAO provides object-oriented access to the sensor table.  This
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
class SensorDAO extends Phreezable
{
	/** @var string */
	public $SensorId;

	/** @var string */
	public $Omschrijving;

	/** @var string */
	public $Eenheid;

	/** @var string */
	public $Omrekenfactor;

	/** @var string */
	public $Presentatie;


	/**
	 * Returns a dataset of AlarmRegel objects with matching Sensor
	 * @param Criteria
	 * @return DataSet
	 */
	public function GetAlarmRegels($criteria = null)
	{
		return $this->_phreezer->GetOneToMany($this, "FK_alarm_regel_sensor", $criteria);
	}

	/**
	 * Returns a dataset of Observatie objects with matching Sensor
	 * @param Criteria
	 * @return DataSet
	 */
	public function GetObservaties($criteria = null)
	{
		return $this->_phreezer->GetOneToMany($this, "FK_observatie_sensor", $criteria);
	}


}
?>