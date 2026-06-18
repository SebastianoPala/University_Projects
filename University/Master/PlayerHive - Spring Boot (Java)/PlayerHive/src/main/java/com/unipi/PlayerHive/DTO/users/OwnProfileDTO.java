package com.unipi.PlayerHive.DTO.users;

import com.unipi.PlayerHive.DTO.users.friends.FriendRequestDTO;
import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.List;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter
public class OwnProfileDTO {


    private String username;

    private String role;

    private String email;

    private String pfpURL;

    private int numGames;

    private float hoursPlayed;

    private LocalDate birthdate;

    private LocalDateTime registrationDate;

    private Integer friends;

    private List<FriendRequestDTO> friendRequests;

    private Integer requestsNum;

}
