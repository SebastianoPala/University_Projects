package com.unipi.PlayerHive.DTO.users.friends;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.springframework.data.mongodb.core.mapping.Field;

import java.time.LocalDateTime;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter
public class FriendRequestDTO {
    @Field("user_id")
    private String userId;
    private String username;
    private String pfpURL;
    private LocalDateTime timestamp;
}
