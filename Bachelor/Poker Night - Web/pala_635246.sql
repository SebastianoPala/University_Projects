-- Progettazione Web 
DROP DATABASE if exists pala_635246; 
CREATE DATABASE pala_635246; 
USE pala_635246; 
-- MySQL dump 10.13  Distrib 5.7.28, for Win64 (x86_64)
--
-- Host: localhost    Database: pala_635246
-- ------------------------------------------------------
-- Server version	5.7.28

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `stanze`
--

DROP TABLE IF EXISTS `stanze`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `stanze` (
  `ID` char(6) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL,
  `pubblica` tinyint(1) NOT NULL DEFAULT '0',
  `giocatori` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `max giocatori` tinyint(1) unsigned NOT NULL DEFAULT '4',
  `storia_mosse` text,
  `turn` tinyint(1) DEFAULT NULL,
  `turn_timestamp` timestamp NULL DEFAULT NULL,
  `stato_partita` text,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `utenti`
--

DROP TABLE IF EXISTS `utenti`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `utenti` (
  `username` varchar(18) NOT NULL,
  `password_hash` varchar(255) NOT NULL,
  `soldi` int(10) unsigned NOT NULL DEFAULT '20000',
  `stanza` text,
  PRIMARY KEY (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `utenti`
--


/*!40000 ALTER TABLE `utenti` DISABLE KEYS */;
INSERT INTO `utenti` VALUES ('alessio','$2y$10$QqUSWBfhNa0TYkMSZqcoWO9F2k7n5HmXgcPPntchRilOMzIwyQpYu',77569,NULL),('bruno','$2y$10$EzoTzUJzvX1iVSSKW2Psp.v637nnSk63Gxf8wCeR9TYEuO7nmrQT2',176493,NULL),('enrico','$2y$10$rPfa//PPkme3zh96Ev4qVexqWkkGbr/KLxzvOr6taadtB2eNnEmXC',88715,NULL),('giorgio','$2y$10$VdlO9Da78xn4qZ/NHKgS3Ocw3fN01hIPRbqeLbPZoaaZjjPH4mawq',35242,NULL),('lara','$2y$10$D4mOMYY1Bw7bU593iH.INOF.Q.et.p.qrhOhjuaH2HLEi/L251OoS',66260,NULL),('Lucia','$2y$10$gwjLsYWjujy2qYdzquULPO/2gAqsJwQxAat/o.VQM6m9SJeVSrCG2',17301,NULL),('marco','$2y$10$KdB3SoztPp.pDTvNbFSmzui8WX8QTxoH/67qwzOJ6CP58snYuf5Wi',12556,NULL),('mario','$2y$10$HxqxEqinvK7qDQ2nXLeeZOEsB6bQcGDyDmRwRGXHrO3abKEpm1h4.',77322,NULL),('matteo','$2y$10$j00sE8RS0GhFG8aWZ3j1Te6r2yuvApknw/Jw6P4PzyhjHGGJKDLcG',0,NULL),('nanni','$2y$10$7VNCGEYkWwjQtB62LiGnJO2Q2HcghgGOH7764sWGNkxhDjpvczfEC',14432,NULL),('nora','$2y$10$qc1c37phmbEpE0XQ1zfqDOw40mHATikF9k7c.WzwxTCJg8uVRdVue',2453421,NULL),('osvaldo','$2y$10$UhJUblLvVJiF9DgUawFcFOazca6ewv3kUbkAtLf8RpJRCKNrLcrdO',54632,NULL),('seba','$2y$10$FXDQRtRazAcNcUKSHGjnjud4T9tch79H5N.8IIuszyaW16B4SnGVe',167995,NULL),('silvano','$2y$10$o4.X1zbAe2ECPZF/PUKwgOCUbqKYp3e3x4TbBAjoiTNQJwNNcmy0u',555676,NULL),('stefano','$2y$10$7RTRvMOSDfwOd5f3im/CmeCiXwFC8BBKBgGQM6d/kB1vs1M2wszem',86489,NULL),('stella','$2y$10$l0rs00nqvH0N5aqMFxVvn.D94ObY0oX8hvPDvVnkzQeEKgAgAZmRq',99876,NULL),('tomas','$2y$10$8cI4g5wd.KhAva7Tt649LOspnY4ifoW7PtEBz5HCMyIaJcCVFIaau',202242,NULL);
/*!40000 ALTER TABLE `utenti` ENABLE KEYS */;

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-09-04 16:06:17
