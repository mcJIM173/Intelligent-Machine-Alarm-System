CREATE TABLE `MAS_STATUS`(
  id INT NOT NULL AUTO_INCREMENT,
        `flame` INT NOT NULL,
        `tilt` INT NOT NULL,
        `motor` INT NOT NULL,
        `power` INT NOT NULL,
        `save_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
        PRIMARY KEY(`id`)
);