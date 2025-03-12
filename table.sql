CREATE TABLE if not exists `friends` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `user_id` int unsigned NOT NULL,
  `friend_id` int unsigned NOT NULL,
  `status` tinyint NOT NULL DEFAULT '1' COMMENT '关系状态 (1: 好友, 0: 待验证, -1: 拉黑)',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `updated_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_unique_friendship` (`user_id`,`friend_id`),
  KEY `idx_user` (`user_id`),
  KEY `idx_friend` (`friend_id`),
  CONSTRAINT `friends_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `userinfo` (`account`) ON DELETE CASCADE,
  CONSTRAINT `friends_ibfk_2` FOREIGN KEY (`friend_id`) REFERENCES `userinfo` (`account`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=91 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci

CREATE TABLE if not exists `userinfo` (
  `id` int NOT NULL AUTO_INCREMENT,
  `account` int unsigned DEFAULT NULL,
  `name` varchar(15) DEFAULT NULL,
  `password` varchar(20) DEFAULT NULL,
  `email` varchar(20) DEFAULT NULL,
  `image` mediumblob,
  `createtime` date NOT NULL DEFAULT '2025-02-02',
  PRIMARY KEY (`id`),
  UNIQUE KEY `account` (`account`)
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci

CREATE TABLE if not exists `offline_messages` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `message_id` text NOT NULL,
  `sender_id` int unsigned NOT NULL,
  `receiver_id` int unsigned NOT NULL,
  `message` mediumblob NOT NULL,
  `status` tinyint NOT NULL DEFAULT '0',
  `created_at` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `md5_hash` char(32) DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `idx_receiver` (`receiver_id`),
  KEY `idx_sender` (`sender_id`),
  CONSTRAINT `offline_messages_ibfk_1` FOREIGN KEY (`sender_id`) REFERENCES `userinfo` (`account`) ON DELETE CASCADE,
  CONSTRAINT `offline_messages_ibfk_2` FOREIGN KEY (`receiver_id`) REFERENCES `userinfo` (`account`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=287 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci

CREATE TABLE if not exists `images` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `md5_hash` char(32) NOT NULL,
  `image_data` mediumblob NOT NULL,
  `upload_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `unique_md5` (`md5_hash`),
  KEY `idx_md5_hash` (`md5_hash`)
) ENGINE=InnoDB AUTO_INCREMENT=68 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci