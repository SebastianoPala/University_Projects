package com.unipi.PlayerHive.DTO.users;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;

import java.time.LocalDateTime;

@NoArgsConstructor
@AllArgsConstructor
@Getter
@Setter
public class ProfileDTO {

    private String username;

    private String role;

    private String pfpURL;

    private int numGames;

    private float hoursPlayed;

    private Integer friends;

    private LocalDateTime registrationDate;

}
