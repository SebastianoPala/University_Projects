package com.unipi.PlayerHive.DTO.users.friends;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.bson.types.ObjectId;
import org.springframework.data.mongodb.core.mapping.Field;

import java.time.LocalDateTime;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter
public class FriendRequestMongoDTO {
    @Field("user_id")
    private ObjectId userId;
    private String username;
    private String pfpURL;
    private LocalDateTime timestamp;
}
