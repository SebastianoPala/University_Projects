package com.unipi.PlayerHive.DTO.users.friends;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class FriendDTO {
    private String id;
    private String username;
    private String pfpURL;
}
